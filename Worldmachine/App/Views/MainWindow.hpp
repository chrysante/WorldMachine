#include "Framework/Window.hpp"

#include <utl/memory.hpp>
#include <filesystem>
#include <optional>

namespace worldmachine {
	
	class Network;
	class BuildSystem;
	
	class MainWindow: public Window {
	public:
		MainWindow();
		~MainWindow();
		
	private:
		void display() override;
		bool keyDown(KeyEvent key) override;
		
		void menuBar();
		
		void buildNetwork();
		void buildNetworkSelected();
		
		void saveFile();
		void showSaveFilePanel();
		void openFile(std::string_view);
		void showOpenFilePanel();
		
	private:
		utl::unique_ref<Network> network;
		utl::unique_ref<BuildSystem> buildSystem;
		std::optional<std::filesystem::path> currentFilePath;
		
#if WM_DEBUGLEVEL
		bool _showImGuiDemo = false;
#endif
	};
	
}
