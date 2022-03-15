#include "ImageDisplayView.hpp"

#include <imgui/imgui.h>
#include <utl/typeinfo.hpp>
#include <mtl/mtl.hpp>

#include "Framework/Window.hpp"
#include "Framework/UIUtil.hpp"

#include "Core/Debug.hpp"
#include "Core/Network/NodeImplementation.hpp"
#include "Core/BuildSystemFwd.hpp"
#include "Core/Image/Image.hpp"


#include "ImageRenderer2D.hpp"

using namespace mtl;

namespace worldmachine {
	
	ImageDisplayView::ImageDisplayView(Network* network):
		View("Image View"),
		NodeView(network)
	{}
	
	void ImageDisplayView::init() {
		
	}
	
	void ImageDisplayView::display() {
		NodeImplementation const* activeNode = getActiveDisplayNodeImplementationPointer();
		
		if (!activeNode) {
			activeNode = getActiveNodeImplementationPointer();
			if (!activeNode) {
				displayInactive("No Node Selected");
				return;
			}
		}
		else if (activeNode->type() != NodeType::image) {
			displayInactive(utl::format("Selected Node is of incompatible type '{}'",
										utl::dynamic_nameof(*activeNode)));
			return;
		}
		// static cast because dynamic_cast has problems across shared lib boundaries.
		// check above asserts the correct type
		auto const* const activeImageNode = static_cast<ImageNodeImplementation const*>(activeNode);

		if (activeImageNode->isBuilding()) {
			if (activeImageNode->currentBuildType() == BuildType::preview &&
				currentRenderedNodeID == activeImageNode->nodeID() &&
				this->hasCachedImage())
			{
				displayImage();
				return;
			}
			float const spinnerRadius = 20;
			ImGui::SetCursorPos(size() / 2 - spinnerRadius);
			progressSpinner("Building", spinnerRadius, 2, getWindow()->appearance().textColor);
			getWindow()->invalidate();
			return;
		}
		if (activeImageNode->built()) {
			maybeUpdateImage(activeImageNode->highResImage(0));
			currentRenderedNodeID = activeImageNode->nodeID();
			this->displayImage(); /* virtual call to child */
			return;
		}
		if (activeImageNode->previewBuilt()) {
			maybeUpdateImage(activeImageNode->previewImage(0));
			currentRenderedNodeID = activeImageNode->nodeID();
			this->displayImage(); /* virtual call to child */
			
//			ImGui::SetCursorPos({ 10, 10 });
//			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 255, 255));
//			ImGui::Text("Preview");
//			ImGui::PopStyleColor();
			return;
		}
		getWindow()->sendMessage(BuildRequest{
			BuildType::preview,
			network(),
			{ activeImageNode->nodeID() }
		});
		
		displayInactive(utl::format("Node '{}' ist not built", activeImageNode->implementationName()));
		return;
	}
	
	void ImageDisplayView::maybeUpdateImage(Image const& img) {
		auto const hash = worldmachine::imageHash({ img.data(), img.size().fold(utl::multiplies) },
												  1024 << 3);
		if (this->imageHash != hash) {
			this->imageHash = hash;
			this->updateImage(img); /* virtual call to child */
		}
	}

}
