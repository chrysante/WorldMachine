#pragma once

#include <mtl/mtl.hpp>
#include <string_view>

namespace worldmachine {
	
	void progressSpinner(std::string_view label,
						 float radius,
						 int thickness,
						 mtl::double4 color);
	
}
