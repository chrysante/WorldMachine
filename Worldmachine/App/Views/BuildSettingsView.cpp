#include "BuildSettingsView.hpp"

#include <imgui/imgui.h>
#include <mtl/mtl.hpp>

#include "Core/Network/Network.hpp"
#include "Core/BuildSystem.hpp"
#include "Framework/Window.hpp"

using namespace mtl::short_types;

namespace worldmachine {
	
	void BuildSettingsView::display() {
		int numThreads = (int)buildSystem->getNumberOfThreads();
		ImGui::InputInt("Worker Threads", &numThreads);
		auto isValid = [](int n) { return n > 0 && n <= std::thread::hardware_concurrency(); };
		if (numThreads != buildSystem->getNumberOfThreads() && isValid(numThreads)) {
			if (!buildSystem->isBuilding()) {
				buildSystem->setNumberOfThreads(numThreads);
			}
		}
		
		setResolution("Build Resolution", true);
		setResolution("Preview Resolution", false);
		
		
	}
	
	static int maxRes(bool highRes) {
		return highRes ? 1 << 13 : 512;
	}
	
	void BuildSettingsView::setResolution(std::string_view header, bool highRes) {
		ImGui::Text("%s", header.data());
		ImGui::Checkbox("Square", &locked);
		int2 resolution = highRes ? buildSystem->getResolution() : buildSystem->getPreviewResolution();
		bool update = false;
		if (ImGui::DragInt(highRes ? "X":"X_", &resolution.x, 1, 32, maxRes(highRes))) {
			if (locked) {
				resolution.y = resolution.x;
			}
			update = true;
		}
		if (!locked && ImGui::DragInt(highRes?"Y":"Y_", &resolution.y, 1, 32, maxRes(highRes))) {
			update = true;
		}
		
		if (update) {
			getWindow()->sendMessage(BuildCancelRequest{});
			network->invalidateAllNodes(highRes ? BuildType::highResolution : BuildType::preview);
			if (highRes) {
				buildSystem->setResolution(resolution);
			}
			else {
				buildSystem->setPreviewResolution(resolution);
			}
		}
	}
	
}

