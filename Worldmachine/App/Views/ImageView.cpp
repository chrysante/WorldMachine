#include "ImageView2D.hpp"

#include <imgui/imgui.h>
#include <typeinfo>
#include <mtl/mtl.hpp>

#include "Core/Debug.hpp"
#include "Core/Network/NodeImplementation.hpp"
#include "Core/Network/Network.hpp"
#include "Core/Network/BuildSystem.hpp"
#include "Core/Image/Image.hpp"

#include "ImageRenderer2D.hpp"

using namespace mtl;

namespace worldmachine {
	
	ImageDisplayView::ImageDisplayView(Network* network, BuildSystem* buildSystem):
		View("Image View"),
		NodeView(network),
		buildSystem(buildSystem)
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
		
		if (activeNode->type() != NodeType::image) {
			return displayInactive(utl::format("Selected Node is of incompatible type '{}'",
											   typeid(*activeNode).name()));
		}
		// static cast because dynamic_cast has problems across shared lib boundaries.
		// check above asserts the correct type
		auto const* const imageNode = static_cast<ImageNodeImplementation const*>(activeNode);
		
		if (buildSystem->isBuilding()) {
			if (buildSystem->currentBuildType() == BuildType::preview &&
				currentRenderedNodeID == imageNode->nodeID())
			{
				if (this->displayImage() /* virtual call to child */) {
					return;
				}
			}
			displayInactive("Building...");
			
			ImGui::SetCursorPosX(this->size().x * 0.25);
			ImGui::ProgressBar(buildSystem->progress(), float2(this->size().x / 2, 0));
			return;
		}
		
		long const nodeIndex = network()->indexFromID(imageNode->nodeID());
		WM_Assert(nodeIndex < (long)network()->nodes().size());
		
		if (nodeIndex < 0) {
			displayInactive("Invalid Node selected");
			return;
		}
		
		auto const nodeFlags = network()->nodes().get<Node::Flags>(nodeIndex);
		
		if (test(nodeFlags & NodeFlags::built)) {
			maybeUpdateImage(imageNode->highResImage(0));
			currentRenderedNodeID = imageNode->nodeID();
			this->displayImage(); /* virtual call to child */
			return;
		}
		else if (test(nodeFlags & NodeFlags::previewBuilt)) {
			maybeUpdateImage(imageNode->previewImage(0));
			currentRenderedNodeID = imageNode->nodeID();
			this->displayImage(); /* virtual call to child */
			
			ImGui::SetCursorPos({ 10, 10 });
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 255, 255));
			ImGui::Text("Preview");
			ImGui::PopStyleColor();
			return;
		}
		else {
			buildSystem->buildPreview(network(), { imageNode->nodeID() });
			displayInactive(utl::format("Node '{}' ist not built", imageNode->implementationName()));
			return;
		}
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
