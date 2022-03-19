#include "Core/Plugin.hpp"

#include <imgui/imgui.h>
#include <utl/math.hpp>

namespace worldmachine {
	
	enum struct CombinerMode: int {
		avg = 0,
		add, multiply, subtract, divide,
		min, max,
		pow, screen, COUNT
	};
	
	constexpr char const* const combinerModeNames[] {
		"Average",
		"Add",
		"Multiply",
		"Subtract",
		"Divide",
		"Minimum",
		"Maximum",
		"Power",
		"Screen"
	};
	
	class CombinerNode: public ImageNodeImplementationT<CombinerNode, "Combiner"> {
	public:
		CombinerNode();
		static NodeDescriptor staticDescriptor();
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override;
		bool displayControls() override;
		
	private:
		CombinerMode mode = CombinerMode::add;
		float strength = 1;
	};
	
	WM_RegisterNode(CombinerNode);
	
	CombinerNode::CombinerNode() {
		serializer().addMember((int*)&mode, "Mode");
		serializer().addMember(&strength, "Strength");
	}
	
	
	BuildJob CombinerNode::makeBuildJob(NodeDependencyMap dependencies) {
		BuildJob result;
		ImageView<float const> inputA = dependencies.getInput<float>(0);
		ImageView<float const> inputB = dependencies.getInput<float>(1);
		ImageView<float> dest = getBuildDest(0);
		
		WM_Assert(dest.size() == inputA.size());
		
		if (!inputB) {
			result.add([=]{
				std::memcpy(dest.data(), inputA.data(), dest.size().fold(utl::multiplies) * sizeof(float));
			});
			return result;
		}
		
		WM_Assert(dest.size() == inputB.size());
		
		auto dispatch = [&](utl::invocable_r<float, float, float> auto&& f){
			result.add([=, strength = strength]{
				std::size_t const size = dest.size().fold(utl::multiplies);
				for (std::size_t i = 0; i < size; ++i) {
					dest[i] = f(inputA[i], inputB[i]);
				}
			});
		};
		
		switch (mode) {
			case CombinerMode::avg:
				dispatch([weight = strength](float a, float b){
					return (1.0f - weight) * a + weight * b;
				});
				break;
			case CombinerMode::add:
				dispatch([strength = strength](float a, float b){
					return a + b * strength;
				});
				break;
			case CombinerMode::multiply:
				dispatch([strength = strength](float a, float b){
					return a * utl::mix(1.0f, b, strength);
				});
				break;
			case CombinerMode::subtract:
				dispatch([strength = strength](float a, float b){
					return a - b * strength;
				});
				break;
			case CombinerMode::divide:
				dispatch([strength = strength](float a, float b){
					return a / utl::mix(1.0f, b, strength);
				});
				break;
			case CombinerMode::min:
				dispatch([strength = strength](float a, float b){
					return std::min(a, utl::mix(1.0f, b, strength));
				});
				break;
			case CombinerMode::max:
				dispatch([strength = strength](float a, float b){
					return std::max(a, utl::mix(0.0f, b, strength));
				});
				break;
			case CombinerMode::pow:
				dispatch([strength = strength](float a, float b){
					return std::pow(a, utl::mix(1.0f, b, strength));
				});
				break;
			case CombinerMode::screen:
				dispatch([strength = strength](float a, float b){
					return utl::screen_blend(a, b * strength);
				});
				break;
			default:
				WM_Log(error, "Unknown Combiner Mode [{}]", (int)mode);
				break;
		}
		return result;
	}
	
	bool CombinerNode::displayControls() {
		bool result = false;
		result |= ImGui::Combo("Mode", (int*)&mode, combinerModeNames, (int)CombinerMode::COUNT);
		
		result |= ImGui::DragFloat(mode == CombinerMode::avg ? "Weight" : "Strength", &strength, 0.001);
		return result;
	}
	
	NodeDescriptor CombinerNode::staticDescriptor() {
		return {
			.category = NodeCategory::filter,
			.name = "Combiner",
			.pinDescriptorArray = {
				.input = {
					{ "A", DataType::float1, mandatory },
					{ "B", DataType::float1 }
				},
				.output = {
					{ "Default", DataType::float1 }
				},
				.parameterInput = {
					{ "Mode", DataType::none },
					{ "Strength", DataType::none }
				},
				.maskInput = {
					{ "Mask", DataType::float1 }
				}
			}
		};
	}
	
	
}
