#pragma once


#include "base.hpp"
UTL(SYSTEM_HEADER)

/* MUST NOT #include "debug.hpp", as it depends on strcat(...) */

#include <string>
#include <sstream>
#include <iosfwd>

namespace utl {
	
	
	
	namespace _private {
		template <typename T>
		concept _printable = requires(std::ostream& s, T&& t) { s << t; };
	}
	
	
	[[nodiscard]] std::string strcat(_private::_printable auto&& f, _private::_printable auto&&... r) {
		std::stringstream sstr;
		sstr << UTL_FORWARD(f);
		((sstr << UTL_FORWARD(r)), ...);
		return std::move(sstr).str();
	}
	
}
