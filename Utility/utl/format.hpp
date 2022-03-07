#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "__private/fmt/format.h"
#include "__private/fmt/ostream.h"


namespace utl {
	
	template <typename... Args>
	UTL(NODISCARD) std::string format(fmt::format_string<Args...> format_string, Args&&... args) {
		return fmt::format(std::move(format_string), UTL_FORWARD(args)...);
	}
}
