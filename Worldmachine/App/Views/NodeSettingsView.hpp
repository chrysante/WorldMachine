#pragma once

#include "NodeView.hpp"

namespace worldmachine {
	
	class NodeSettingsView: public NodeView {
	public:
		NodeSettingsView(Network* network):
			View("Node Settings"),
			NodeView(network)
		{}

		void display() override;
	};
	
}
