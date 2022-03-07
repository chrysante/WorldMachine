#define IMGUI_DEFINE_MATH_OPERATORS

#include "PluginsView.hpp"

#include "Core/PluginManager.hpp"
#include "Core/Registry.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace worldmachine {
	
	void PluginsView::display() {
		using namespace ImGui;
		
		Button("Load Plugin...");
		
		Separator();
		
		auto loadedPlugins = PluginManager::instance().getLoadedPlugins();
		for (auto& plugin: loadedPlugins) {
			auto& g = *GImGui;
			SetCursorPos(GetCursorPos() + ImVec2{ 0, g.Style.FramePadding.y });
			Text("%s", plugin.name().data());
			SameLine();
			if (Button("Reload")) {
				PluginManager::instance().refreshPlugin(plugin, network);
			}
		}
		
		Separator();
		
		Text("Registry Entries");
		Registry::instance().displayEntries();
	}
	
}
