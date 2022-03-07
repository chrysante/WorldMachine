#pragma once

#include <string>

namespace worldmachine {
	
	class Network;
	
	std::string serializeNetwork(Network const&);
	bool deserializeNetwork(Network&, std::string_view);
	
}
