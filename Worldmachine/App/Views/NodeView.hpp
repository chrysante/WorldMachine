#pragma once

#include "Framework/View.hpp"

#include <utl/memory.hpp>
#include <utl/UUID.hpp>
#include <optional>

namespace worldmachine {
	
	class Network;
	class NodeImplementation;
	
	class NodeView: public virtual View {
	protected:
		NodeView(Network* network):
			View(std::string{}),
			_network(network)
		{}
	
	public:
		void setActiveNode(std::optional<utl::UUID> id) {
			_activeNodeID = id;
		}
		void setActiveDisplayNode(std::optional<utl::UUID> id) {
			_activeDisplayNodeID = id;
		}
		
	protected:
		long getActiveNodeIndex() { return _getIndex(_activeNodeID); }
		long getActiveDisplayNodeIndex() { return _getIndex(_activeDisplayNodeID); }
		
		std::optional<utl::UUID> getActiveNodeID() { return _activeNodeID; }
		std::optional<utl::UUID> getActiveDisplayNodeID() { return _activeDisplayNodeID; }
		
		NodeImplementation* getActiveNodeImplementationPointer() { return _getImplPtr(_activeNodeID); }
		NodeImplementation* getActiveDisplayNodeImplementationPointer() { return _getImplPtr(_activeDisplayNodeID); }
		
		Network* network() { return _network; }
		
	private:
		long _getIndex(std::optional<utl::UUID> const&);
		NodeImplementation* _getImplPtr(std::optional<utl::UUID> const&);
		
	private:
		Network* _network;
		std::optional<utl::UUID> _activeNodeID;
		std::optional<utl::UUID> _activeDisplayNodeID;
	};
	
}
