#include "NetworkListView.hpp"

#include <imgui/imgui.h>

#include "Framework/Window.hpp"
#include "Framework/ImGuiExt.hpp"
#include "Core/Network/Network.hpp"

#include "NodeView.hpp"

namespace worldmachine {
	
	void NetworkListView::init() {
		
	}
	
	void NetworkListView::display() {
		using namespace ImGui;
		
		auto const& nodes = network->nodes;
		std::size_t const nodeCount = nodes.size();
		selected.resize(nodeCount);
		std::memset(selected.data(), 0, nodeCount * sizeof(bool));
		for (auto nodeIndex: network->selectedIndices()) {
			selected[nodeIndex] = true;
		}
		
		for (std::size_t i = 0; i < nodeCount; ++i) {
			auto const& name = nodes[i].name;
			if (ImGui::Selectable(name.empty() ? "##" : name.data(), &selected[i])) {
				network->clearSelection();
				network->selectNode(i);
				getWindow()->forEachView([&](View* view) {
					auto* nodeView = dynamic_cast<NodeView*>(view);
					if (!nodeView) {
						return;
					}
					nodeView->setActiveNode(network->IDFromIndex(i));
				});
			}
		}
	}
	
}
