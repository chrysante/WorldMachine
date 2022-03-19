#include "Core/Plugin.hpp"

namespace worldmachine {
	
	class ViewNode: public ImageNodeImplementationT<ViewNode, "View"> {
	public:
		bool displayControls() override { return false; };
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override { return {}; };
		
		static NodeDescriptor staticDescriptor();
	};
	
	WM_RegisterNode(ViewNode);
	
	NodeDescriptor ViewNode::staticDescriptor() {
		return {
			.category = NodeCategory::output,
			.name = "View",
			.pinDescriptorArray = {
				.input = {
					{ "Height", DataType::float1, mandatory },
					{ "Color", DataType::float3 }
				},
				.parameterInput = {
					{ "Mode", DataType::none }
				}
			}
		};
	}
	
	
}
