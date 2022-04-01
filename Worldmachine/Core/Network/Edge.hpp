#pragma once

#include "mtl/mtl.hpp"
#include "utl/named_type.hpp"
#include "utl/structure_of_arrays.hpp"

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
	
//	struct Edge {
//		using BeginNodeIndex = utl::named_type<std::size_t, struct BeginNodeIndexTag>;
//		using EndNodeIndex   = utl::named_type<std::size_t, struct EndNodeIndexTag>;
//		using BeginPinIndex  = utl::named_type<std::size_t, struct BeginPinIndexTag>;
//		using EndPinIndex    = utl::named_type<std::size_t, struct EndPinIndexTag>;
//		using BeginPinKind   = utl::named_type<PinKind,     struct BeginPinKindTag>;
//		using EndPinKind     = utl::named_type<PinKind,     struct EndPinKindTag>;
//		using Proxy          = utl::named_type<EdgeProxy,   struct EdgeProxyTag>;
//	};
	
	struct PinIndex {
		std::size_t   nodeIndex;
		std::size_t   pinIndex;
		PinKind pinKind;
	};
	
//	using EdgeType = std::tuple<
//		Edge::BeginNodeIndex,
//		Edge::EndNodeIndex,
//		Edge::BeginPinIndex,
//		Edge::EndPinIndex,
//		Edge::BeginPinKind,
//		Edge::EndPinKind,
//		Edge::Proxy
//	>;
	
}
