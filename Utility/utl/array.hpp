#pragma once


#include "base.hpp"
UTL(SYSTEM_HEADER)
#include <array>

namespace utl {
	
	
	template <typename T, std::size_t Size>
	using array = std::array<T, Size>;
	
}
