#include "NetworkTraversal.hpp"

#include "Network.hpp"

#include <utl/hashset.hpp>

namespace worldmachine {
	
	NetworkIterator::NetworkIterator(Network const* network,
									 std::size_t beginIndex,
									 Direction dir,
									 bool traverseUnique):
		visited(network->nodes.size()),
		traverseUnique(traverseUnique)
	{
		edges.reserve(network->edges.size());
		std::copy(network->edges.begin(), network->edges.end(), std::back_inserter(edges));
		if (dir == NetworkIterator::Direction::upstream) {
			// reverse edges to traverse upstream
			std::transform(edges.begin(), edges.end(),
						   edges.begin(), [](EdgeRep edge) {
				return reverse(edge);
			});
		}
		std::sort(edges.begin(), edges.end(), [](EdgeRep a, EdgeRep b) {
			return a.begin < b.begin;
		});
		ancestors.push(utl::narrow_cast<IndexType>(beginIndex));
	}
	
	void NetworkIterator::advance() {
		while (!done()) {
			visited[current()] = true;
			
			// find child that we have not visited yet
			auto itr = std::lower_bound(edges.begin(),
										edges.end(),
										EdgeRep{ current(), (IndexType)-1 },
										[](EdgeRep a, EdgeRep b) {
				 return a.begin < b.begin;
			});
			
			if (itr == edges.end() || itr->begin > current()) {
				// we have visited all children
				// pop this node from the stack
				// and make parent current
				ancestors.pop();
				continue;
			}
			
			WM_Assert(itr->begin == current());
			
			// erase edge
			EdgeRep const edge = *itr;
			edges.erase(itr);

			if (traverseUnique && visited[edge.end]) {
				continue;
			}
			
			ancestors.push(edge.end);
			return;
		}
	}
	
	bool hasCycles(Network* network, std::size_t beginIndex) {
		auto view = NetworkTraversalView(network, beginIndex);
		for (auto i = view.begin(); i != view.end(); ++i) {
			auto& stack = i.ancestors.container();
			auto const begin = stack.begin();
			auto const end = stack.end() - 1;
			if (std::find(begin, end, *i) != end) {
				// we have a cycle
				return true;
			}
		}
		return false;
	}
	
}
