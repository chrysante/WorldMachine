#define IMGUI_DEFINE_MATH_OPERATORS
#define UTL_DEFER_MACROS

#include "DebugConsole.hpp"
#include "Core/Debug.hpp"

#include <utl/typeinfo.hpp>
#include <mtl/mtl.hpp>
#include <utl/scope_guard.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "Framework/ImGuiExt.hpp"
#include "Framework/Appearance.hpp"
#include "Framework/Window.hpp"

using namespace mtl;

namespace worldmachine {
	
	DebugConsole::DebugConsole(): View("Logs") {
		setPadding(2);
	}
	
	void DebugConsole::columnItem(int index, mtl::float4 const& color, std::string_view text, bool wrapText) {
		ImGui::TableSetColumnIndex(index);
		auto const currentCursorPosition = ImGui::GetCursorPos();
		if (wrapText)
			ImGui::PushTextWrapPos();
		ImGui::TextColored(color, "%s", text.data());
		if (wrapText)
			ImGui::PopTextWrapPos();
		float const textWidth = ImGui::CalcTextSize(text.data()).x;
		if (!wrapText && ImGui::IsItemHovered() && textWidth > ImGui::GetColumnWidth()) {
			auto position = ImGui::GetWindowPos() + currentCursorPosition - GImGui->Style.WindowPadding;
			
			position.x += std::min(0.0, getWindow()->size().x - (position.x + textWidth +
																 2 * GImGui->Style.WindowPadding.x));
			position.y -= ImGui::GetScrollY();
			ImGui::SetNextWindowPos(position);
			ImGui::BeginTooltip();
			ImGui::Text("%s", text.data());
			ImGui::EndTooltip();
		}
	}
	
	void DebugConsole::columnItem(int index, mtl::float4 const& color, std::string_view format, auto&&... args) {
		columnItem(index, color, utl::format(format, UTL_FORWARD(args)...));
	}
	
	
	void DebugConsole::display() {
		

		auto const font = ImGui::GetIO().Fonts->Fonts[2];
		WM_Assert(font);
		ImGui::PushFont(font);
		if (ImGui::BeginTable("Console Logs", 6,
							  ImGuiTableFlags_Resizable |
							  ImGuiTableFlags_Hideable  |
							  ImGuiTableFlags_ScrollY))
		{
			ImGui::TableSetupColumn("Module");
			ImGui::TableSetupColumn("Time");
			ImGui::TableSetupColumn("Thread");
			ImGui::TableSetupColumn("File");
			ImGui::TableSetupColumn("Function");
			ImGui::TableSetupColumn("Message");
			ImGui::TableHeadersRow();
			
			auto [lock, logs] = globalLog();
			for (auto& logMessage: logs) {
				ImGui::TableNextRow();
				auto const& a = getWindow()->appearance();
				auto textColor = [&](utl::log_level l) { return a.logTextColors[utl::log2((unsigned)l)]; };
				columnItem(0, textColor(utl::log_level::trace), logMessage.logger_name);
				columnItem(1, textColor(utl::log_level::trace), utl::time_in_HH_MM_SS_MMM(logMessage.time));
				columnItem(2, textColor(utl::log_level::trace),
						   logMessage.thread_id == std::this_thread::get_id() ?
						   "main" : utl::format("{}", logMessage.thread_id));
				columnItem(3, textColor(utl::log_level::trace), logMessage.source_info.file);
				columnItem(4, textColor(utl::log_level::trace), "{}: {}", utl::qualified_function_name(logMessage.source_info.function),
						   logMessage.source_info.line);
				columnItem(5, textColor(logMessage.level), logMessage.message, true);
			}
			if (currentLines != logs.size()) {
				currentLines = logs.size();
				ImGui::SetScrollHereY(1.0f);
			}
			lock.unlock();
			ImGui::EndTable();
		}
		ImGui::PopFont();
		
		
	}
	
}
