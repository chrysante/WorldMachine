#pragma once

#include <string>
#include <filesystem>
#include <utl/vector.hpp>
#include <utl/dynamic_library.hpp>
#include <utl/UUID.hpp>
#include <span>
#include "PluginManagerFwd.hpp"

namespace worldmachine {
	
	struct Plugin {
		Plugin(utl::dynamic_library);
		
		utl::UUID id() const { return _id; }
		std::string_view name() const { return _name; }
		std::string_view buildTime() const { return _buildTime; }
		
		bool operator==(Plugin const& rhs) const;
		
	private:
		friend class PluginManager;
		friend class Registry;
		utl::UUID _id;
		std::string _name;
		std::string _buildTime;
		utl::dynamic_library lib;
	};
	
	class PluginManager {
	public:
		static PluginManager& instance();
		
		std::span<Plugin> getLoadedPlugins() { return loadedPlugins; }
		void loadPlugin(std::filesystem::path);
		void refreshPlugin(Plugin&);
		
		Plugin* getPlugin(utl::UUID id);
		
	private:
		utl::vector<Plugin> loadedPlugins;
	};
	
}
