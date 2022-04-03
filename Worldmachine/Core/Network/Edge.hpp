#pragma once

#include <utl/structure_of_arrays.hpp>

#include "Pin.hpp"
#include "SharedNetworkTypes.hpp"

namespace worldmachine {
	
	inline EdgeParameters defaultEdgeParameters() {
		return {
			.width = 5
		};
	}
	
	UTL_SOA_TYPE(Edge,
				 (std::size_t, beginNodeIndex),
				 (std::size_t, endNodeIndex),
				 (std::size_t, beginPinIndex),
				 (std::size_t, endPinIndex),
				 (PinKind,     beginPinKind),
				 (PinKind,     endPinKind),
				 (EdgeProxy,   proxy)
				 );
	
	struct PinIndex {
		std::size_t   nodeIndex;
		std::size_t   pinIndex;
		PinKind pinKind;
	};
	
}
