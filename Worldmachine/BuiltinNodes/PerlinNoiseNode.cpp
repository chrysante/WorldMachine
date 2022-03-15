#include "Core/Plugin.hpp"

#include <imgui/imgui.h>
#include <mtl/mtl.hpp>
#include <utl/md_array.hpp>
#include <utl/stdio.hpp>
#include <utl/stopwatch.hpp>
#include <utl/scope_guard.hpp>
#include <utl/dynamic_dispatch.hpp>
#include <utl/math.hpp>
#include <random>

using namespace mtl;

namespace worldmachine {
	
	enum struct PerlinNoiseInterpolation: int {
		linear, cubic, quintic, COUNT
	};
	
	constexpr char const* const perlinNoiseInterpolationNames[] = {
		"Linear", "Cubic", "Quintic"
	};
	
	struct PerlinNoiseParameters {
		float scale = 1;
		int seed = 0;
		int levels = 1;
		float strengthFalloff = 0.5;
		float scaleFalloff = 1.7;
		float uvOffsetStrength = 0.5;
		PerlinNoiseInterpolation interpolation = PerlinNoiseInterpolation::quintic;
	};
	
	class PerlinNoiseNode: public ImageNodeImplementationT<PerlinNoiseNode, "Perlin Noise"> {
	public:
		PerlinNoiseNode();
		bool displayControls() override;
		
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override;
		
		static NodeDescriptor staticDescriptor();
		
	private:
		PerlinNoiseParameters params{};
	};
	
	WM_RegisterNode(PerlinNoiseNode);
	
	PerlinNoiseNode::PerlinNoiseNode() {
		serializer().addMember(&params.scale, "Scale");
		serializer().addMember(&params.seed, "Seed");
		serializer().addMember(&params.levels, "Levels");
		serializer().addMember(&params.strengthFalloff, "Strength Falloff");
		serializer().addMember(&params.scaleFalloff, "Scale Falloff");
		serializer().addMember(&params.uvOffsetStrength, "UV Offset Strength");
		serializer().addMember((int*)&params.interpolation, "Interpolation");
	}
	
	bool PerlinNoiseNode::displayControls() {
		bool result = false;
		result |= ImGui::DragFloat("Scale", &params.scale, .05, .1, 100);
		result |= ImGui::DragInt("Seed", &params.seed);
		result |= ImGui::DragInt("Levels", &params.levels, .1, 1, 8);
		result |= ImGui::DragFloat("Scale Falloff", &params.scaleFalloff, 0.01, 1, 2);
		result |= ImGui::DragFloat("Strength Falloff", &params.strengthFalloff, 0.01, 0, 1);
		result |= ImGui::DragFloat("UV Offset Strength", &params.uvOffsetStrength, 0.01, 0, 1);
		
		result |= ImGui::Combo("Interpolation", (int*)&params.interpolation,
							   perlinNoiseInterpolationNames, (int)PerlinNoiseInterpolation::COUNT);
		
		return result;
	}
	
	namespace {
		
		struct BuildData {
			utl::small_vector<float2, 16> scaleData;
			utl::small_vector<float, 16> strengthData;
			utl::vector<utl::md_array<utl::vector<float>, 2>> pointData;
		};
	
		void perlinNoise(ImageView<float> img, std::size_t yStart, std::size_t yEnd,
						 int level, float2 levelScale, float levelStrength,
						 BuildData* data,
						 utl::invocable_r<float2, float2> auto&& interpolation,
						 utl::invocable_r<float2, float2, std::size_t, std::size_t> auto&& offsetUV)
		{
			for (std::size_t y = yStart; y < yEnd; ++y) {
				for (std::size_t x = 0, xSize = img.size().x; x < xSize; ++x) {
					float2 const uv = levelScale * float2{ x, y } / img.size();
					
					float2 const fcoord = interpolation(utl::fract(uv));
					int2 icoord = utl::floor(uv);
					if (levelScale.x > img.size().x) {
						icoord.x = (int)x;
					}
					if (levelScale.y > img.size().y) {
						icoord.y = (int)y;
					}
					  
					auto const value = [&]{
						auto const lower = [&]{
							auto const left = data->pointData[level](icoord);
							auto const right = data->pointData[level](icoord + int2{ 1, 0 });
							return utl::mix(left, right, fcoord.x);
						}();
						auto const upper = [&]{
							auto const left = data->pointData[level](icoord + int2{ 0, 1 });
							auto const right = data->pointData[level](icoord + int2{ 1, 1 });
							return utl::mix(left, right, fcoord.x);
						}();
						return utl::mix(lower, upper, fcoord.y);
					}();
					float& dst = img(x, y);
					dst += value * levelStrength;
				}
			}
		}
		
		void leveledPerlinNoise(ImageView<float> img, std::size_t yStart, std::size_t yEnd,
								PerlinNoiseParameters params,
								BuildData* data,
								auto&& interpolation,
								utl::invocable_r<float2, float2, std::size_t, std::size_t> auto&& offsetUV)
		{
			  for (int i = 0; i < params.levels; ++i) {
				  perlinNoise(img, yStart, yEnd, i,
							  data->scaleData[i], data->strengthData[i],
							  data,
							  interpolation,
							  offsetUV);
			  }
		}
		
		constexpr size_t hash(usize2 s) {
			size_t result = 0xcbf29ce484222325; // FNV offset basis

			for (char i : s) {
				result ^= i * 1099511628211ul * 1099511628211ul;
				result *= 1099511628211ul; // FNV prime
				result *= 1099511628211ul; // FNV prime
			}

			return result;
		}
		
		utl::md_array<utl::vector<float>, 2> calculatePointData(usize2 imgSize, float2 scale, int seed) {
			utl::md_array<utl::vector<float>, 2> pointData(std::max((std::size_t)std::ceil(scale.x), imgSize.x) + 1,
														   std::max((std::size_t)std::ceil(scale.y), imgSize.y) + 1);
			auto const totalPoints = pointData.size().fold(utl::multiplies);
			if (totalPoints > 8*8*1024*1024) {
				throw BuildError(utl::format("we cant calculate this many points ({})", totalPoints));
			}
			for (auto index: utl::iota<usize2>(pointData.size())) {
				std::size_t hashValue = hash(index + seed * 1099511628211ul);
				pointData[index] = (float)hashValue / ULONG_MAX;
			}
			return pointData;
		};
		
	}
	
	BuildJob PerlinNoiseNode::makeBuildJob(NodeDependencyMap dependencies) {
		ImageView<float> dest = getBuildDest(0);
		BuildJob job;
		auto* const data = new BuildData();
		job.onCleanup([data]{
			delete data;
		});
		
		std::size_t const rowsPerJob = 32;
		
		float const aspectRatio = (float)dest.size().x / dest.size().y;
		float2 const scale = { this->params.scale * aspectRatio, this->params.scale };
		
		std::tie(data->scaleData, data->strengthData) = [&]{
			utl::small_vector<float2, 16> scales(params.levels);
			for (auto i = scale; auto& s: scales) {
				s = i;
				i *= params.scaleFalloff;
			}
			
			utl::small_vector<float, 16> strength(params.levels);
			for (float i = 1; auto& s: strength) {
				s = i;
				i *= params.strengthFalloff;
			};
			float const sum = std::accumulate(strength.begin(), strength.end(), 0.0f);
			for (auto& s: strength) s /= sum;
			
			return std::pair{ scales, strength };
		}();
		
		data->pointData.reserve(params.levels);
		for (int i = 0; i < params.levels; ++i) {
			data->pointData.emplace_back(calculatePointData(dest.size(), data->scaleData[i], params.seed + i));
		}
		
		
		auto linear = [](float2 v) {
			return v;
		};
		auto cubic = [](float2 f) {
			return f * f * (3 - 2 * f);
		};
		auto quintic = [](float2 f) {
			return f*f*f*(f*(f*6.0f-15.0f)+10.0f);
		};
		
		auto noUVOffset = [](float2 uv, auto, auto) { return uv; };
		
		utl::dispatch(utl::dispatch_arg((int)params.interpolation, linear, cubic, quintic),
					  utl::dispatch_arg(0, noUVOffset),
					  [&](auto interpolation, auto uvOffset) {
			for (std::size_t yStart = 0; yStart < dest.size().y; yStart += rowsPerJob) {
				std::size_t const yEnd = std::min<std::size_t>(yStart + rowsPerJob, dest.size().y);
				job.add([=]{
					leveledPerlinNoise(dest, yStart, yEnd, params, data,
									   interpolation,
									   uvOffset);
				});
			}
		});
		
		
		return job;
	}
	
	
	
	NodeDescriptor PerlinNoiseNode::staticDescriptor() {
		return {
			.name = "Perlin Noise",
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

