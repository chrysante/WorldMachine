#include "NodeView.hpp"

#include "Core/Network/Network.hpp"

namespace worldmachine {
	
	long NodeView::_getIndex(std::optional<utl::UUID> const& id) {
		if (!id) {
			return -1;
		}
		return _network->indexFromID(*id);
	}
	
	NodeImplementation* NodeView::_getImplPtr(std::optional<utl::UUID> const& id) {
		if (!id) {
			return nullptr;
		}
		auto const nodeIndex = _getIndex(id);
		if (nodeIndex == -1) {
			return nullptr;
		}
		return _network->nodes().get<Node::Implementation>(nodeIndex).get();
	}
	
}
