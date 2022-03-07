#include "PluginManager.hpp"

#include <imgui/imgui.h>
#include <yaml-cpp/yaml.h>

#include "Registry.hpp"
#include "Core/Network/Network.hpp"

namespace worldmachine {
	
	/// MARK: - Plugin
	Plugin::Plugin(utl::dynamic_library l):
		_id(utl::UUID::generate()),
		lib(std::move(l))
	{
		// get name
		auto getNameFn = lib.symbol_ptr<char const*()>("WM_Plugin_GetName");
		_name = getNameFn();
		
		// set imgui data
		auto setImGuiDataFn = lib.symbol_ptr<void(worldmachine_ImGuiRuntimeData)>("WM_Plugin_SetImGuiData");
		setImGuiDataFn(getImGuiRuntimeData());
		
		// build time
		auto getBuildTimeFn = lib.symbol_ptr<char const*()>("WM_Plugin_BuildTime");
		_buildTime = getBuildTimeFn();
		
		// merge registries
		auto getRegistryFn = lib.symbol_ptr<void const*()>("WM_Plugin_GetRegistry");
		Registry pluginRegistry = *static_cast<Registry const*>(getRegistryFn());
		for (auto& [_, vtable]: pluginRegistry.vtables) {
			vtable._pluginID = this->id();
		}
		Registry::instance().merge(pluginRegistry);
	}
	
	bool Plugin::operator==(Plugin const& rhs) const {
		if (this->lib == rhs.lib) {
			WM_Assert(this->id() == rhs.id());
			return true;
		}
		WM_Assert(this->id() != rhs.id());
		return false;
	}
	
	
	/// MARK: - PluginManager
	PluginManager& PluginManager::instance() {
		static PluginManager pm;
		return pm;
	}
	
	void PluginManager::loadPlugin(std::filesystem::path path) {
		WM_Log("Loading plugin {}", path);
		try {
			loadedPlugins.push_back(utl::dynamic_library(path));
		}
		catch(std::exception const& e) {
			WM_Log(error, "Plugin Corrupted\n{}", e.what());
			return;
		}
	}
	
	void PluginManager::refreshPlugin(Plugin& plugin, Network* network) {
		[[maybe_unused]]
		auto itr = std::find(loadedPlugins.begin(),
							 loadedPlugins.end(),
							 plugin);
		WM_Assert(itr != loadedPlugins.end());
		
		// save the network
		utl::vector<std::string> nodeTexts;
		utl::vector<ImplementationID> implIDs;
		network->nodes().for_each<Node::Implementation>([&](Node::Implementation& impl) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			impl->serializer().serialize(out);
			out << YAML::EndMap;
			
			nodeTexts.push_back(out.c_str());
			implIDs.push_back(impl->implementationID());
			impl.reset();
		});
		
		Registry::instance().eraseWithPluginID(plugin.id());
		plugin.lib.close();
		plugin = Plugin(utl::dynamic_library(plugin.lib.current_path()));
		
		std::size_t textIndex = 0;
		network->nodes().for_each<Node::Implementation, Node::ID>([&](Node::Implementation& impl,
																	  auto nodeID) {
			ImplementationID const implementationID = implIDs[textIndex];
			impl = Registry::instance().createNodeImplementation(implementationID, nodeID);
			
			YAML::Node yamlNode = YAML::Load(nodeTexts[textIndex]);
			impl->serializer().deserialize(yamlNode);
			
			++textIndex;
		});
		network->invalidateAllNodes();
	}
	
	Plugin* PluginManager::getPlugin(utl::UUID id) {
		auto const itr = std::find_if(loadedPlugins.begin(), loadedPlugins.end(), [&](Plugin const& p) {
			return p.id() == id;
		});
		if (itr == loadedPlugins.end()) {
			return nullptr;
		}
		return &*itr;
	}
	
}
