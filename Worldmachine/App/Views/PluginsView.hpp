#pragma once

#include "Framework/View.hpp"

namespace worldmachine {
	
	class Network;
	
	class PluginsView: public View {
	public:
		PluginsView():
			View("Plugins")
		{}

		void display() override;
	};
	
}

