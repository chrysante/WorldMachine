#pragma once

#include "ImageView.hpp"

#include <utl/memory.hpp>

namespace worldmachine {
	
	class ImageRenderer2D;
	
	class ImageView2D: public ImageDisplayView {
	public:
		ImageView2D(Network*, BuildSystem*);

		void init() override;
		
	private:
		void updateImage(Image const&) override;
		bool displayImage() override;
	private:
		utl::unique_ref<ImageRenderer2D> renderer;
	};
	
}
