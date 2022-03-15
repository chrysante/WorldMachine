#include "Core/Plugin.hpp"

#include <imgui/imgui.h>
#include <utl/md_array.hpp>
#include <mtl/mtl.hpp>
#include <utl/dynamic_dispatch.hpp>
#include <random>

using namespace mtl;

namespace worldmachine {
	
	enum struct VoronoiDistanceFunction: int {
		euclidian = 0, sum, max, pnorm, COUNT
	};
	
	char const* const voronoiDistanceFunctionNames[] = {
		"Euclidian", "Sum (Manhatten)", "Maximum (Chebychev)", "P-Norm"
	};
	
	struct VoronoiParameters {
		float scale = 1;
		int seed = std::rand();
		float modulation = 0.5;
		float uvOffsetStrength = 0.5;
		VoronoiDistanceFunction distanceFunction = VoronoiDistanceFunction::euclidian;
		float p = 1;
		bool squareHeight = true;
	};
	
	
	class VoronoiNode: public ImageNodeImplementationT<VoronoiNode, "Voronoi"> {
	public:
		VoronoiNode();
		
		bool displayControls() override;
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override;
		static NodeDescriptor staticDescriptor();
		
	private:
		VoronoiParameters params{};
	};
	
	WM_RegisterNode(VoronoiNode);
	
	/// MARK: - Implementation
	
	VoronoiNode::VoronoiNode() {
		serializer().addMember(&params.scale, "Scale");
		serializer().addMember(&params.seed, "Seed");
		serializer().addMember(&params.modulation, "Modulation");
		serializer().addMember(&params.uvOffsetStrength, "UV Offset Strength");
		serializer().addMember((int*)&params.distanceFunction, "Distance Function");
		serializer().addMember(&params.p, "P");
		serializer().addMember(&params.squareHeight, "Square Height");
	}
	
	bool VoronoiNode::displayControls() {
		bool result = false;
		result |= ImGui::DragFloat("Scale", &params.scale, .05, .1, 100);
		result |= ImGui::DragInt("Seed", &params.seed);
		result |= ImGui::DragFloat("Modulation", &params.modulation, 0.01, 0, 1);
		result |= ImGui::DragFloat("UV Offset Strength", &params.uvOffsetStrength, 0.01, 0, 2);
		result |= ImGui::Combo("Distance Function",
							   (int*)&params.distanceFunction,
							   voronoiDistanceFunctionNames,
							   (int)VoronoiDistanceFunction::COUNT);
		if (params.distanceFunction == VoronoiDistanceFunction::pnorm) {
			result |= ImGui::DragFloat("P", &params.p, 0.01, 1, 100);
		}
		result |= ImGui::Checkbox("Square Height", &params.squareHeight);
		return result;
	}
	
	namespace {
		
		struct BuildData {
			utl::md_array<utl::vector<float3>, 3> pointData;
		};
	
		template <bool squareHeight, bool hasUVOffset>
		void algorithm(ImageView<float> img, std::size_t yStart, std::size_t yEnd,
					   VoronoiParameters params, BuildData const* data,
					   utl::invocable_r<float, float3, float3> auto&& distanceFunction,
					   utl::invocable_r<float2, float2, std::size_t, std::size_t> auto&& offsetUV)
		{
			for (std::size_t y = yStart; y < yEnd; ++y) {
				for (std::size_t x = 0, xSize = img.size().x; x < xSize; ++x) {
					mtl::float2 const uv = params.scale * mtl::float2{ x, y } / img.size();
					mtl::float2 const distUV = offsetUV(uv, x, y);
					mtl::float2 const fcoord = utl::fract(distUV);
					mtl::int3 const icoord = mtl::concat(mtl::floor(distUV), 0);
					
					float value = std::numeric_limits<float>::max();
					
					for (auto [i, j, k]: utl::iota<mtl::int3>(-1, 2)) {
						mtl::uint3 const index = icoord + mtl::uint3{ i + 1, j + 1, k + 1 };
						if constexpr (hasUVOffset) {
							if (!mtl::map(index, data->pointData.size(), utl::less).fold(utl::logical_and)) {
								continue;
							}
							if (!mtl::map(index, mtl::int3{ 0 }, utl::greater_eq).fold(utl::logical_and)) {
								continue;
							}
						}
						
						auto const p = data->pointData(index);
						auto const dist = distanceFunction(mtl::concat(fcoord, params.modulation) - mtl::float3(i, j, k), p);
						value = std::min(value, dist);
					}
					if constexpr (squareHeight) {
						img(x, y) = value * value;
					}
					else {
						img(x, y) = value;
					}
					
				}
			}
		}
		
		template <typename T, std::size_t N>
		utl::md_array<utl::vector<T>, N> calculatePointData(int seed, mtl::vector<int, N> size){
			auto engine = std::mt19937_64(seed);
			std::uniform_real_distribution<float> dist;
			
			utl::md_array<utl::vector<T>, N> pointData(size);
			
			for (auto& p: pointData) {
				p = T([&]{ return dist(engine); });
			}
			return pointData;
		}
		
	}
	
	BuildJob VoronoiNode::makeBuildJob(NodeDependencyMap dependencies) {
		ImageView<float> dest = getBuildDest(0);
		
		std::size_t const rowsPerJob = 8;
		
		float const aspectRatio = (float)dest.size().x / dest.size().y;
		mtl::float2 const scale = { params.scale * aspectRatio, params.scale };
		
		BuildJob job;
		auto* const data = new BuildData();
		job.onCleanup([data]{
			delete data;
		});
		data->pointData = calculatePointData<float3, 3>(params.seed, int3(scale.map(utl::ceil) + 2, 3));

		ImageView<float2 const> uvOffsetImage = dependencies.getInput<float2>(0);
		
		
		for (std::size_t yStart = 0; yStart < dest.size().y; yStart += rowsPerJob) {
			std::size_t const yEnd = std::min<std::size_t>(yStart + rowsPerJob, dest.size().y);
			
			/// Possible UV Offset functions
			bool const hasUVOffset = !!uvOffsetImage;
			auto uvOffset = [uvOffsetImage,
							 strength = params.uvOffsetStrength] (mtl::float2 uv, std::size_t x, std::size_t y)
			{
				return uv + strength * (uvOffsetImage(x, y) - 0.5f);
			};
			auto noUVOffset = [](float2 uv, std::size_t, std::size_t){ return uv; };
			
			/// Possible Distance Functions
			auto euclDist = [](float3 const& a, float3 const& b) {
				return mtl::fast_distance(a, b);
			};
			auto sumDist = [](float3 const& a, float3 const& b) {
				return (a - b).map(utl::abs).fold(utl::plus);
			};
			auto maxDist = [](float3 const& a, float3 const& b) {
				return (a - b).map(utl::abs).fold(utl::max);
			};
			auto pnormDist = [p = params.p](float3 const& a, float3 const& b) {
				// basically mtl::fast_pnorm(a - b)
				return std::pow((a - b).map(utl::abs).map([&](auto const& x) {
					return std::pow(x, p);
				}).fold(utl::plus), 1.0f / p);
			};
			
			utl::dispatch(utl::dispatch_arg((int)params.distanceFunction,
											euclDist, sumDist, maxDist, pnormDist),
						  utl::dispatch_arg(hasUVOffset, noUVOffset, uvOffset),
						  utl::dispatch_condition(hasUVOffset),
						  utl::dispatch_condition(params.squareHeight),
						  [&](auto distanceFunction,
							  auto uvOffset,
							  auto hasUVOffset,
							  auto squareHeight) {
				bool constexpr SH = decltype(squareHeight)::value;
				bool constexpr O  = decltype(hasUVOffset)::value;
				job.add([=, params = this->params]{
					algorithm<SH, O>(dest, yStart, yEnd, params, data, distanceFunction, uvOffset);
				});
			});
		}
		return job;
	}
	
	NodeDescriptor VoronoiNode::staticDescriptor() {
		return {
			.name = "Voronoi",
			.category = NodeCategory::generator,
			.pinDescriptorArray = {
				.input = {
					{ "UV Offset", DataType::float2 }
				},
				.output = {
					{ "Default", DataType::float1 }
				},
				.parameterInput = {
					{ "Scale", DataType::none },
					{ "Seed", DataType::none },
					{ "Iterations", DataType::none }
				},
				.maskInput = {
					{ "Mask", DataType::float1 }
				}
			}
		};
	}
	
}
