#pragma once

#include "Framework/View.hpp"

namespace worldmachine {
	
	class Network;
	
	class PluginsView: public View {
	public:
		PluginsView(Network* network):
			View("Plugins"),
			network(network)
		{}

		void display() override;
		
	private:
		Network* network;
	};
	
}

