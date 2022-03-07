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
		
		auto const& nodes = network->nodes();
		std::size_t const nodeCount = nodes.size();
		selected.resize(nodeCount);
		std::memset(selected.data(), 0, nodeCount * sizeof(bool));
		for (auto nodeIndex: network->selectedIndices()) {
			selected[nodeIndex] = true;
		}
		
		for (std::size_t i = 0; i < nodeCount; ++i) {
			auto const& name = nodes.get<Node::Name>(i);
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
		
		
		
//		if (BeginTable("Nodes", 5, flags)) {
//			TableSetupColumn("Name");
//			TableSetupColumn("VTable Pointer");
//			TableHeadersRow();
//			for (auto& [implementationID, vtable]: vtables) {
//				ImGui::TableNextRow();
//				auto desc = vtable.descriptor();
//				auto impl = vtable.createImplementation();
//				void* implVTablePtr;
//				std::memcpy(&implVTablePtr, (void*)impl.get(), sizeof(void*));
//				ImGui::TableSetColumnIndex(0);
//				ImGui::Textf("{}", desc.name);
//				ImGui::TableSetColumnIndex(1);
//				ImGui::Textf("{}", implementationID);
//				ImGui::TableSetColumnIndex(2);
//				ImGui::Textf("{}", vtable.pluginID());
//				ImGui::TableSetColumnIndex(3);
//				ImGui::Textf("{}", vtable.buildTime());
//				ImGui::TableSetColumnIndex(4);
//				ImGui::Textf("{}", implVTablePtr);
//			}
//			ImGui::EndTable();
//		}
	}
	
}
