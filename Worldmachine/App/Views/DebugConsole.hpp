#pragma once

#include "Framework/View.hpp"

#include <utl/vector.hpp>
#include <string>
#include <sstream>

namespace worldmachine {
	
	class DebugConsole: public View {
	public:
		DebugConsole();
		
		void display() override;
		
	private:
		void columnItem(int index,
						mtl::float4 const& color,
						std::string_view text,
						bool wrapText = false);
		
		void columnItem(int index,
						mtl::float4 const& color,
						std::string_view format,
						auto&&... args);
		
	private:
		std::size_t currentLines = 0;
	};
	
}
