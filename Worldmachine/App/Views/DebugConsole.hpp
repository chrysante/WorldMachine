#pragma once

#include "Framework/View.hpp"

#include <utl/vector.hpp>
#include <string>
#include <sstream>

namespace worldmachine {
	
	class ConsoleBuffer {
	public:
		ConsoleBuffer();
		
		static ConsoleBuffer* instance;
		std::stringstream sstr;
	};

	class DebugConsole: public View {
	public:
		DebugConsole();
		
		void display() override;
		
	private:
		utl::vector<std::string> lines;
	};
	
}
