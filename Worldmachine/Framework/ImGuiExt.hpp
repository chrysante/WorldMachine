#pragma once

#include <mtl/mtl.hpp>
#include <string_view>
#include <utl/format.hpp>

_Pragma("GCC system_header")

namespace ImGui {
	
	void Matrix(std::string_view label, mtl::float4x4 x);
	void Matrix(std::string_view label, mtl::double4x4 x);
	
	template <typename... Args>
	void Textf(std::string_view formatString, Args&&... args) {
		Text("%s", utl::format(formatString, UTL_FORWARD(args)...).data());
	}
	
}
