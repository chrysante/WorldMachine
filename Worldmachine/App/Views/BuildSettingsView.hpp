#pragma once

#include "Framework/View.hpp"

namespace worldmachine {
	
	class BuildSystem;
	class Network;
	
	class BuildSettingsView: public View {
	public:
		BuildSettingsView(Network* network, BuildSystem* buildSystem):
			View("Build Settings"),
			network(network),
			buildSystem(buildSystem)
		{}

		void display() override;
	
	private:
		void setResolution(std::string_view header, bool highRes);
		
	private:
		Network* network;
		BuildSystem* buildSystem;
		bool locked = true;
	};
	
}
