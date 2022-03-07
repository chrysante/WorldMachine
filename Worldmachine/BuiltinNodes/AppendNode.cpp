#include "Core/Plugin.hpp"

#include <mtl/mtl.hpp>
#include <imgui/imgui.h>

using namespace mtl::short_types;

namespace worldmachine {
	
	class AppendNode: public ImageNodeImplementationT<AppendNode, "Append"> {
	public:
		bool displayControls() override { return false; };
		BuildJob makeBuildJob(NodeDependencyMap dependencies) override;
		static NodeDescriptor staticDescriptor();
	};
	
	WM_RegisterNode(AppendNode);
	
	BuildJob AppendNode::makeBuildJob(NodeDependencyMap dependencies) {
		BuildJob result;
		ImageView<float const> inputA = dependencies.getInput<float>(0);
		ImageView<float const> inputB = dependencies.getInput<float>(1);
		ImageView<float2> dest = this->getBuildDest(0);
		WM_Assert(dest.size() == inputA.size());
		WM_Assert(dest.size() == inputB.size());
		result.add([inputA, inputB, dest]{
			std::size_t const flatSize = dest.size().fold(utl::multiplies);
			for (std::size_t i = 0; i < flatSize; ++i) {
				dest[i] = float2(inputA[i], inputB[i]);
			}
		});
		return result;
	}
	
	NodeDescriptor AppendNode::staticDescriptor() {
		return {
			.name = "Append",
			.category = NodeCategory::filter,
			.pinDescriptorArray = {
				.input = {
					{ "Red Channel", DataType::float1, mandatory },
					{ "Green Channel", DataType::float1, mandatory }
				},
				.output = {
					{ "Default", DataType::float2 }
				}
			}
		};
	}
	
	
}
