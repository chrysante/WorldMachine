#include "NodeSettingsView.hpp"

#include <imgui/imgui.h>

#include "Core/Network/Network.hpp"
#include "Core/Network/NodeImplementation.hpp"
#include "Core/Network/BuildSystem.hpp"

namespace worldmachine {

	void NodeSettingsView::display() {
		auto* const activeNode = getActiveNodeImplementationPointer();
		auto* activeDisplayNode = getActiveDisplayNodeImplementationPointer();
		if (!activeDisplayNode) {
			activeDisplayNode = activeNode;
		}
		
		if (!activeNode) {
			return displayInactive("No Node Selected");
		}
		else if (buildSystem->isBuilding() && buildSystem->currentBuildType() == BuildType::highResolution) {
			return displayInactive("Building...");
		}
		
		/// Name input
		auto const nodeIndex = network()->indexFromID(activeNode->nodeID());
		auto const& nodeName = network()->nodes().get<Node::Name>(nodeIndex);
		std::size_t constexpr bufferLength = 256;
		char buffer[bufferLength]{};
		std::copy(nodeName.begin(),
				  std::min(nodeName.end(), nodeName.begin() + bufferLength),
				  buffer);
		
		if (ImGui::InputText("Name", buffer, bufferLength)) {
			network()->setNodeName(nodeIndex, buffer);
		}
		ImGui::Separator();
		if (buildSystem->locked([&]{ return activeNode->displayControls(); })) {
			if (buildSystem->isBuilding()) {
				buildSystem->cancelCurrentBuild();
			}
			network()->invalidateNodesDownstream(activeNode->nodeID());
			
			buildSystem->buildPreview(network(), { activeDisplayNode->nodeID() });
		}
	}

}
