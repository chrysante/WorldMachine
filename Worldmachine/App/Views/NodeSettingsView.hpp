#pragma once

#include "NodeView.hpp"

namespace worldmachine {
	
	class BuildSystem;
	
	class NodeSettingsView: public NodeView {
	public:
		NodeSettingsView(Network* network, BuildSystem* buildSystem):
			View("Node Settings"),
			NodeView(network),
			buildSystem(buildSystem)
		{}

		void display() override;
		
	private:
		BuildSystem* buildSystem;
	};
	
}
