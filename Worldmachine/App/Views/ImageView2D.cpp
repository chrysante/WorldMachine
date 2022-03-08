#include "ImageView2D.hpp"

#include <imgui/imgui.h>
#include <mtl/mtl.hpp>

#include "Core/Image/Image.hpp"

#include "ImageRenderer2D.hpp"

using namespace mtl;

namespace worldmachine {
	
	ImageView2D::ImageView2D(Network* network):
		View("Image View 2D"),
		ImageDisplayView(network)
	{}
	
	void ImageView2D::init() {
		ImageDisplayView::init();
		renderer = utl::make_unique_ref<ImageRenderer2D>();
	}
	
	void ImageView2D::updateImage(Image const& img) {
		renderer->draw(img);
	}
	
	void ImageView2D::displayImage() {
		usize2 const size = renderer->renderedImageSize();
		auto const ratio = (double2)size / size.fold(utl::max);
		auto const displaySize = ratio * (this->size() / ratio).fold(utl::min);
		
		ImGui::SetCursorPos((this->size() - displaySize) / 2);
		worldmachine::displayImage(renderer->renderedImage(), displaySize);
	}
	
	bool ImageView2D::hasCachedImage() const {
		return renderer->renderedImage() != nullptr;
	}
	
}
