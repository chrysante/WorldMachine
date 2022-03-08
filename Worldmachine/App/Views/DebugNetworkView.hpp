#pragma once

#include "NetworkView.hpp"

namespace worldmachine {
	
	class DebugNetworkView: public NetworkView {
	public:
		DebugNetworkView(Network* network):
			View("Network View"),
			NetworkView(network)
		{}
		
	private:
		void display() override;
		
		void optionMenuItem(bool&, std::string_view label);
		
		void colorOptionsPanel();
		void portalParametersPanel();

		
		void rightMouseDragged(MouseDragEvent) override;
		void scrollWheel(ScrollEvent) override;
		void mouseMoved(MouseMoveEvent) override;
		
		mtl::double2 mouseLocation = 0;
		utl::function<void()> mouseMoveInfo;
		utl::function<void()> mouseOffsetInfo;
		
		
		
		bool showColorOptions = false;
		bool showMouseMoveInfo = false;
		bool showPortalParameters = false;
		bool showAppearanceInspector = false;
	};
	
}
