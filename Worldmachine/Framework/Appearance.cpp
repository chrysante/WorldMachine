#define IMGUI_DEFINE_MATH_OPERATORS

#include "Appearance.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

using namespace mtl::short_types;

namespace worldmachine {
	
	static void showColorPicker(Appearance const& a, char const* name, auto field) {
		float2 const size = { 80, 80 };
		ImVec4 color = a.*field;
		ImGui::ColorButton(name, color, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_AlphaPreview, size);
		ImGui::SameLine();
		ImGui::Text("%s", name);
	};
	
	void appearanceInspectPanel(Appearance const& a, bool& show) {
		ImGui::Begin("Appearance Inspector", &show);
		
#define COLOR_PICKER(NAME) showColorPicker(a, #NAME, &Appearance::NAME)
		
		COLOR_PICKER(labelColor);
		COLOR_PICKER(secondaryLabelColor);
		COLOR_PICKER(tertiaryLabelColor);
		COLOR_PICKER(quaternaryLabelColor);
		COLOR_PICKER(textColor);
		COLOR_PICKER(placeholderTextColor);
		COLOR_PICKER(selectedTextColor);
		COLOR_PICKER(textBackgroundColor);
		COLOR_PICKER(selectedTextBackgroundColor);
		COLOR_PICKER(keyboardFocusIndicatorColor);
		COLOR_PICKER(unemphasizedSelectedTextColor);
		COLOR_PICKER(unemphasizedSelectedTextBackgroundColor);
		COLOR_PICKER(linkColor);
		COLOR_PICKER(separatorColor);
		COLOR_PICKER(selectedContentBackgroundColor);
		COLOR_PICKER(unemphasizedSelectedContentBackgroundColor);
		COLOR_PICKER(selectedMenuItemTextColor);
		COLOR_PICKER(gridColor);
		COLOR_PICKER(headerTextColor);
//		COLOR_PICKER(alternatingContentBackgroundColors);
//		COLOR_PICKER(alternatingContentBackgroundColors);
		COLOR_PICKER(controlAccentColor);
		COLOR_PICKER(controlColor);
		COLOR_PICKER(controlBackgroundColor);
		COLOR_PICKER(controlTextColor);
		COLOR_PICKER(disabledControlTextColor);
//		COLOR_PICKER(currentControlTint);
		COLOR_PICKER(selectedControlColor);
		COLOR_PICKER(selectedControlTextColor);
		COLOR_PICKER(alternateSelectedControlTextColor);
		COLOR_PICKER(scrubberTexturedBackground);
		COLOR_PICKER(windowBackgroundColor);
		COLOR_PICKER(windowFrameTextColor);
		COLOR_PICKER(underPageBackgroundColor);
		COLOR_PICKER(findHighlightColor);
		COLOR_PICKER(highlightColor);
		COLOR_PICKER(shadowColor);
		
#undef COLOR_PICKER
		
		ImGui::End();
	}
	
	
	
	void setImGuiStyleColors(Appearance const& a, ImGuiStyle* dst) {
		switch (a.type) {
			case Appearance::Type::light:
				ImGui::StyleColorsLight();
				break;
				
			case Appearance::Type::dark:
				ImGui::StyleColorsDark();
				break;
				
			default:
				ImGui::StyleColorsLight();
				break;
		}
		
		ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text]                   = a.textColor;
		colors[ImGuiCol_TextDisabled]           = a.disabledControlTextColor;
		colors[ImGuiCol_WindowBg]               = a.windowBackgroundColor;
		colors[ImGuiCol_ChildBg]                = a.windowBackgroundColor;
		colors[ImGuiCol_PopupBg]                = a.controlBackgroundColor;
		return;
		colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
		colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
		colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}
	
	
}
