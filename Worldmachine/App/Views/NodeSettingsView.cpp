#include "NodeSettingsView.hpp"

#include <imgui/imgui.h>

#include "Core/Network/Network.hpp"
#include "Core/Network/NodeImplementation.hpp"
#include "Core/BuildSystemFwd.hpp"
#include "Framework/Window.hpp"

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
		else if (network()->isBuilding() && network()->buildInfo().type() == BuildType::highResolution) {
			return displayInactive("Building...");
		}
		
		/// Name input
		auto const nodeIndex = network()->indexFromID(activeNode->nodeID());
		auto const& nodeName = network()->nodes[nodeIndex].name;
		std::size_t constexpr bufferLength = 256;
		char buffer[bufferLength]{};
		std::copy(nodeName.begin(),
				  std::min(nodeName.end(), nodeName.begin() + bufferLength),
				  buffer);
		
		if (ImGui::InputText("Name", buffer, bufferLength)) {
			network()->nodes[nodeIndex].name = buffer;
		}
		ImGui::Separator();
		if (activeNode->displayControls()) {
			if (network()->isBuilding()) {
				getWindow()->sendMessage(BuildCancelRequest{});
			}
			network()->invalidateNodesDownstream(activeNode->nodeID());
			
			getWindow()->sendMessage(BuildRequest{
				BuildType::preview, network(), { activeDisplayNode->nodeID() }
			});
		}
	}

}
