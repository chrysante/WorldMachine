#pragma once

#include "ImageDisplayView.hpp"

#include <utl/memory.hpp>

namespace worldmachine {
	
	class ImageRenderer2D;
	
	class ImageView2D: public ImageDisplayView {
	public:
		ImageView2D(Network*);

		void init() override;
		
	private:
		void updateImage(Image const&) override;
		void displayImage() override;
		bool hasCachedImage() const override;
	private:
		utl::unique_ref<ImageRenderer2D> renderer;
	};
	
}
