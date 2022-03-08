#pragma once

#include "NodeView.hpp"

namespace worldmachine {
	
	
	class ImageNodeImplementation;
	class Image;
	
	class ImageDisplayView:
		public InputSurfaceView,
		public NodeView
	{
	public:
		ImageDisplayView(Network*);
		
		void init() override;
		void display() override;
		
	private:
		virtual void updateImage(Image const&) = 0;
		virtual void displayImage() = 0;
		virtual bool hasCachedImage() const { return false; }
		
		void maybeUpdateImage(Image const&);
		
	private:
		std::size_t imageHash = 0;
		utl::UUID currentRenderedNodeID;
	};
	
}
