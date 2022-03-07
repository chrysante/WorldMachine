#pragma once

#include "NodeView.hpp"

namespace worldmachine {
	
	
	class ImageNodeImplementation;
	class BuildSystem;
	class Image;
	
	class ImageDisplayView:
		public InputSurfaceView,
		public NodeView
	{
	public:
		ImageDisplayView(Network*, BuildSystem*);
		
		void init() override;
		void display() override;
		
	private:
		virtual void updateImage(Image const&) = 0;
		virtual bool displayImage() = 0;
		
		void maybeUpdateImage(Image const&);
		
	private:
		BuildSystem* buildSystem;
		std::size_t imageHash = 0;
		utl::UUID currentRenderedNodeID;
	};
	
}
