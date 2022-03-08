#include "GlobalMessenger.hpp"

namespace worldmachine {
	utl::messenger& globalMessenger() {
		static utl::messenger m;
		return m;
	}

}

