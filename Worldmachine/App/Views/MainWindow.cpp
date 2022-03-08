#include "MainWindow.hpp"

#include <imgui/imgui.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "Framework/ResourceUtil.hpp"

#include "Core/PluginManager.hpp"
#include "Core/Network/Network.hpp"
#include "Core/Network/NetworkSerialize.hpp"
#include "Core/BuildSystem.hpp"

#include "NetworkView.hpp"
#include "DebugNetworkView.hpp"
#include "ImageView2D.hpp"
#include "HeightmapView3D.hpp"
#include "NodeSettingsView.hpp"
#include "BuildSettingsView.hpp"
#include "PluginsView.hpp"
#include "NetworkListView.hpp"
#include "DebugConsole.hpp"

namespace worldmachine {
	
	MainWindow::MainWindow():
		Window("World Machine")
	{
		PluginManager::instance().loadPlugin(executablePath().parent_path() / "libWMBuiltinNodes.dylib");
		
		network = Network::create();
		
		buildSystem = BuildSystem::create();
		buildSystem->setViewInvalidator([this]{
			invalidate();
		});
		auto buildSystemListeners = buildSystem->makeListeners();
		auto ids = getMessenger().register_listeners(buildSystemListeners.begin(), buildSystemListeners.end());
		for (auto& id: ids) storeListenerID(std::move(id));
		
		
#if WM_DEBUGLEVEL
		addView(new DebugNetworkView(network.get()));
#else
		addView(new NetworkView(network.get()));
#endif
		addView(new ImageView2D(network.get()));
		addView(new HeightmapView3D(network.get()));
		addView(new NodeSettingsView(network.get()));
		addView(new BuildSettingsView(network.get(), buildSystem.get()));
		addView(new PluginsView());
		addView(new NetworkListView(network.get()));
		addView(new DebugConsole());
	}
	
	MainWindow::~MainWindow() = default;
	
	void MainWindow::display() {
		menuBar();
	}
	
	bool MainWindow::keyDown(KeyEvent event) {
		switch (event.keyCode) {
			case KeyCode::B:
				if (test(event.modifierFlags & EventModifierFlags::super)) {
					if (test(event.modifierFlags & EventModifierFlags::shift)) {
						buildNetworkSelected();
					}
					else {
						buildNetwork();
					}
					return true;
				}
				return false;
				
			case KeyCode::S:
				if (test(event.modifierFlags & EventModifierFlags::super)) {
					if (currentFilePath) {
						saveFile();
					}
					else {
						showSaveFilePanel();
					}
					return true;
				}
				return false;
				
			case KeyCode::O:
				if (test(event.modifierFlags & EventModifierFlags::super)) {
					showOpenFilePanel();
					return true;
				}
				return false;
				
			case KeyCode::period:
				/// TODO: edit framework to recognize modifiers for this key and change this shortcut to CMD + .
				sendMessage(BuildCancelRequest{});
				return true;
				
			default:
				return false;
		}
	}
	
	void MainWindow::menuBar() {
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open...", "CMD + O")) {
				showOpenFilePanel();
			}
			if (ImGui::MenuItem("Save", nullptr, false, currentFilePath.has_value())) {
				saveFile();
			}
			if (ImGui::MenuItem("Save as...")) {
				showSaveFilePanel();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::EndMenu();
		}
	
		if (ImGui::BeginMenu("Build")) {
			if (ImGui::MenuItem("Build all", "CMD + B", false, !buildSystem->isBuilding())) {
				buildNetwork();
			}
			if (ImGui::MenuItem("Build selected", "CMD + Shift + B", false, !buildSystem->isBuilding())) {
				buildNetworkSelected();
			}
			if (ImGui::MenuItem("Cancel Build", "CMD + .", false, buildSystem->isBuilding())) {
				sendMessage(BuildCancelRequest{});
			}
			
			if (ImGui::MenuItem("Invalidate all")) {
				sendMessage(BuildCancelRequest{});
				network->invalidateAllNodes();
			}
			ImGui::EndMenu();
		}
		
#if WM_DEBUGLEVEL
		if (ImGui::BeginMenu("Debug")) {
			if (ImGui::MenuItem("Show ImGui Demo Window", nullptr,
								_showImGuiDemo))
			{
				_showImGuiDemo ^= true;
			}
			ImGui::EndMenu();
		}
#endif
		
		ImGui::EndMainMenuBar();
	}
	
	void MainWindow::buildNetwork() {
		if (network->isBuilding()) {
			WM_Log(error, "Build Action is currently active");
			return;
		}
		sendMessage(BuildRequest{
			BuildType::highResolution, network.get()
		});
	}
	
	void MainWindow::buildNetworkSelected() {
		if (network->isBuilding()) {
			WM_Log(error, "Build Action is currently active");
			return;
		}
		sendMessage(BuildRequest{
			BuildType::highResolution,
			network.get(),
			network->IDsFromIndices(network->selectedIndices())
		});
	}
	
	
	void MainWindow::saveFile() {
		if (!currentFilePath) {
			WM_Log(warning, "Failed to save file: No file specified");
			return;
		}
		
		std::fstream file(*currentFilePath, std::ios::out | std::ios::trunc);
		if (!file) {
			WM_Log(warning, "Failed to open file \"{}\"", currentFilePath->string());
			return;
		}
		std::string text = serializeNetwork(*network);
		file << text;
	}
	
	void MainWindow::showSaveFilePanel() {
		worldmachine::showSaveFilePanel([this](std::string_view path) {
			currentFilePath = path;
			saveFile();
			invalidate();
		});
	}
	
	void MainWindow::openFile(std::string_view filename) {
		currentFilePath = filename;
		std::fstream file(filename, std::ios::in);
		if (!file) {
			WM_Log(warning, "Failed to open file \"{}\"", filename);
			return;
		}
		std::stringstream sstr;
		sstr << file.rdbuf();
		std::string contents = std::move(sstr).str();
		deserializeNetwork(*network, contents);
		
		if (auto* view = dynamic_cast<NetworkView*>(findViewByName("Network View"))) {
			view->onFileOpen();
		}
	}
	
	void MainWindow::showOpenFilePanel() {
		worldmachine::showOpenFilePanel([this](std::string_view path) {
			currentFilePath = path;
			openFile(path);
			invalidate();
		});
	}
	
}


extern worldmachine::Window* createMainWindow() {
	return new worldmachine::MainWindow();
}
