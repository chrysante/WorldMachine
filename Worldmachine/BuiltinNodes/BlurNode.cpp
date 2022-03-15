#include "BlurNode.hpp"

namespace worldmachine {
	WM_RegisterNode(BlurNode);
	
	NodeDescriptor BlurNode::staticDescriptor() {
		return {
			.category = NodeCategory::filter,
			.name = "Blur",
			.pinDescriptorArray = {
				.input = {
					{ "Primary", DataType::float1, true }
				},
				.output = {
					{ "Primary", DataType::float1 }
				}
			}
		};
	}
	
	bool BlurNode::displayControls() {
		return false;
	}
	
	BuildJob BlurNode::makeBuildJob(NodeDependencyMap) {
		return {};
	}
	
}
