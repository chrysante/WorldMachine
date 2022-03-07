#pragma once

#include <utl/vector.hpp>

#include "Framework/View.hpp"


namespace worldmachine {
	
	class Network;
	
	class NetworkListView: public View {
	public:
		NetworkListView(Network* network): View("Network List View"), network(network) {}
		void init() override;
		void display() override;
		
	private:
		Network* network;
		utl::vector<bool> selected;
	};
	
}
