#include "DebugConsole.hpp"

#include <utl/stdio.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "Framework/ImGuiExt.hpp"

#include <iostream>

namespace worldmachine {
	
	ConsoleBuffer* ConsoleBuffer::instance = new ConsoleBuffer();
	
	ConsoleBuffer::ConsoleBuffer() {
		utl::set_global_output_stream(sstr);
	}
	
	DebugConsole::DebugConsole(): View("Console") {
		
	}
	
	void DebugConsole::display() {
		bool scrollToBottom = false;
		std::stringstream sstr;
		sstr << ConsoleBuffer::instance->sstr.rdbuf();
		std::string line;
		while (std::getline(sstr, line)) {
			lines.push_back(line);
			scrollToBottom = true;
		}
		
		auto const font = ImGui::GetIO().Fonts->Fonts[2];
		assert(font);
		ImGui::PushFont(font);
		
		for (std::string_view line: lines) {
			ImGui::Text("%s", line.data());
		}
		
		ImGui::PopFont();
		
		if (scrollToBottom) {
			ImGui::SetScrollHereY(1.0f);
		}
	}
	
}
