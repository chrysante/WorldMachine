#include "Menus.hpp"

#include <imgui/imgui.h>
#include "Core/Registry.hpp"

using namespace mtl::short_types;

namespace worldmachine {
	
	
	bool CreateNodesMenu::display() {
		if (!_shown) {
			_shown = false;
			// menu just opens up, init here
			
			for (auto& descs: nodeIDs) {
				descs.clear();
			}
			auto& registry = Registry::instance();
			auto const ids = registry.getIDs();
			for (ImplementationID id: ids) {
				NodeDescriptor const desc = registry.createDescriptorFromID(id);
				nodeIDs[(std::size_t)desc.category].push_back(desc);
			}
		}
		
		bool result = false;
		for (std::size_t i = 0;
			 auto& descs: nodeIDs)
		{
			if (ImGui::BeginMenu(utl::format("{}", (NodeCategory)i++).data())) {
				for (NodeDescriptor& desc: descs) {
					if (ImGui::MenuItem(desc.name.data())) {
						_selectedElement = desc;
						result = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndMenu();
			}
		}
		if (result) {
			_shown = false;
		}
		return result;
	}

	
}
