#include "ImageView2D.hpp"

#include <imgui/imgui.h>
#include <mtl/mtl.hpp>

#include "Core/Image/Image.hpp"

#include "ImageRenderer2D.hpp"

using namespace mtl;

namespace worldmachine {
	
	ImageView2D::ImageView2D(Network* network, BuildSystem* buildSystem):
		View("Image View 2D"),
		ImageDisplayView(network, buildSystem)
	{}
	
	void ImageView2D::init() {
		ImageDisplayView::init();
		renderer = utl::make_unique_ref<ImageRenderer2D>();
	}
	
	void ImageView2D::updateImage(Image const& img) {
		renderer->draw(img);
	}
	
	bool ImageView2D::displayImage() {
		if (!renderer->renderedImage()) {
			return false;
		}
		usize2 const size = renderer->renderedImageSize();
		auto const ratio = (double2)size / size.fold(utl::max);
		auto const displaySize = ratio * (this->size() / ratio).fold(utl::min);
		
		ImGui::SetCursorPos((this->size() - displaySize) / 2);
		worldmachine::displayImage(renderer->renderedImage(), displaySize);
		return true;
	}
	
}
