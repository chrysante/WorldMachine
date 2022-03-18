#pragma once

#include <mtl/mtl.hpp>
#include <string_view>
#include <utl/format.hpp>

#if __GNUC__ || __clang__
#	pragma GCC system_header
#endif

namespace ImGui {
	
	void Matrix(std::string_view label, mtl::float4x4 x);
	void Matrix(std::string_view label, mtl::double4x4 x);
	
	template <typename... Args>
	void Textf(std::string_view formatString, Args&&... args) {
		Text("%s", utl::format(formatString, UTL_FORWARD(args)...).data());
	}
	
	template <typename... Args>
	void TextfColored(mtl::float4 color, std::string_view formatString, Args&&... args) {
		TextColored(color, "%s", utl::format(formatString, UTL_FORWARD(args)...).data());
	}
	
	bool SliderDouble2(const char *label,
					   double *v,
					   double v_min,
					   double v_max);
	
}
