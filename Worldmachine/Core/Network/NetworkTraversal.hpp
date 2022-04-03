#pragma once

#include <cstdint>
#include <cstddef>
#include <stack>
#include <optional>
#include <utl/vector.hpp>

#include "Edge.hpp"

namespace worldmachine {
	
	class Network;
	
	bool hasCycles(Network const* network, std::size_t beginIndex);
	
	class NetworkIterator {
		friend bool hasCycles(Network const*, std::size_t);
		using IndexType = std::uint16_t; // to keep the structure small
		
	public:
		struct Sentinel {};
		
		enum struct Direction: std::int8_t {
			downstream, upstream
		};
		
	public:
		NetworkIterator(Network const* network,
						std::size_t beginIndex,
						Direction dir,
						bool traverseUnique);
	
		void advance();
		bool done() const { return ancestors.empty(); }
		std::size_t dereference() const { return current(); }
		
		NetworkIterator& operator++(){
			advance();
			return *this;
		};
		bool operator==(Sentinel) const { return done(); }
		std::size_t operator*() const { return dereference(); }
		
	private:
		IndexType current() const { return ancestors.top(); }
		
		struct EdgeRep {
			EdgeRep(IndexType begin, IndexType end): begin(begin), end(end) {}
			EdgeRep(Edge::reference e): begin(e.beginNodeIndex), end(e.endNodeIndex) {}
			EdgeRep(Edge::const_reference e): begin(e.beginNodeIndex), end(e.endNodeIndex) {}
			
			friend EdgeRep reverse(EdgeRep e) {
				return { e.end, e.begin };
			}
			
			IndexType begin, end;
		};
		
	private:
		utl::vector<EdgeRep> edges;
		std::stack<IndexType, utl::small_vector<IndexType>> ancestors;
		utl::vector<bool> visited;
		bool traverseUnique;
	};
	
	
	
	class NetworkTraversalView {
	public:
		NetworkTraversalView(Network const* network, std::size_t beginIndex):
			network(network), beginIndex(beginIndex)
		{}
		
		NetworkIterator begin() const {
			return NetworkIterator(network,
								   beginIndex,
								   NetworkIterator::Direction::downstream,
								   traverseUnique);
		}
		NetworkIterator::Sentinel end() const { return {}; }
		
		NetworkIterator rbegin() const {
			return NetworkIterator(network,
								   beginIndex,
								   NetworkIterator::Direction::upstream,
								   traverseUnique);
		}
		NetworkIterator::Sentinel rend() const { return {}; }
		
		/*
		 Only operate on temporaries and return by value
		 to work well with range-based for loops.
		 */
		NetworkTraversalView unique()&& {
			traverseUnique = true;
			return *this;
		}
		
	private:
		Network const* network;
		std::size_t beginIndex;
		bool traverseUnique = false;
	};
	
}


