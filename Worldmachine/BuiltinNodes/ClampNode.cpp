#include "Core/Plugin.hpp"

#include <imgui/imgui.h>

namespace worldmachine {
	
	class ClampNode: public ImageNodeImplementationT<ClampNode, "Clamp"> {
	public:
		ClampNode();

		static NodeDescriptor staticDescriptor();
		
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override;
		bool displayControls() override;
		
	private:
		float min = 0, max = 1;
	};
	
	WM_RegisterNode(ClampNode);
	
	ClampNode::ClampNode() {
		serializer().addMember(&min, "Min");
		serializer().addMember(&max, "Max");
	}
	
	bool ClampNode::displayControls() {
		bool result = false;
		result |= ImGui::SliderFloat("Minimum", &min, 0, 1);
		min = std::clamp(min, 0.0f, max);
		result |= ImGui::SliderFloat("Maximum", &max, 0, 1);
		max = std::clamp(max, min, 1.0f);
		return result;
	}
	
	BuildJob ClampNode::makeBuildJob(NodeDependencyMap dependencies) {
		clearBuildDest();
		ImageView<float> dest = getBuildDest(0);
		
		auto const input = dependencies.getInput<float>(0);

		WM_Assert(input.size() == dest.size());
		
		BuildJob result;
		result.add([input, dest, min = this->min, max = this->max]{
			float const range = max - min;
			auto const flatSize = dest.size().fold(utl::multiplies);
			for (std::size_t i = 0; i < flatSize; ++i) {
				dest[i] = input[i] * range + min;
			}
		});
		
		return result;
		
	}
	
	NodeDescriptor ClampNode::staticDescriptor() {
		return {
			.category = NodeCategory::filter,
			.name = "Clamp",
			.pinDescriptorArray = {
				.input = {
					{ "Input", DataType::float1, mandatory }
				},
				.output = {
					{ "Default", DataType::float1 }
				},
				.parameterInput = {
					{ "Min", DataType::none },
					{ "Max", DataType::none },
					{ "Mode", DataType::none }
				}
			}
		};
	}
	
	
}
