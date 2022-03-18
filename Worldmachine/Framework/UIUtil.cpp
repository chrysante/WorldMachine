#include "UIUtil.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <mtl/mtl.hpp>

namespace worldmachine {
	
	void progressSpinner(std::string_view label, float radius, int thickness, mtl::double4 color) {
		using namespace ImGui;
			ImGuiWindow* window = GetCurrentWindow();
			if (window->SkipItems)
				return;
			
			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID(label.data());
			
			ImVec2 pos = window->DC.CursorPos;
			ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);
			
			const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
			ItemSize(bb, style.FramePadding.y);
			if (!ItemAdd(bb, id))
				return;
			
			// Render
			window->DrawList->PathClear();
			
			int num_segments = 30;
			int start = abs(std::sin(g.Time * 1.8f) * (num_segments - 5));
			
			const float a_min = mtl::constants<>::pi * 2.0f * ((float)start) / (float)num_segments;
			const float a_max = mtl::constants<>::pi * 2.0f * ((float)num_segments - 3) / (float)num_segments;

			const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius+style.FramePadding.y);
			
			for (int i = 0; i < num_segments; i++) {
				const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
				window->DrawList->PathLineTo(ImVec2(centre.x + std::cos(a + g.Time * 8) * radius,
													centre.y + std::sin(a + g.Time * 8) * radius));
			}

			window->DrawList->PathStroke(ColorConvertFloat4ToU32(color), false, thickness);
		}
	
}
