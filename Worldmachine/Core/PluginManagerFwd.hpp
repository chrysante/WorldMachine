#pragma once

#include <utl/messenger.hpp>

namespace worldmachine {
	
	class PluginManager;
	
	struct PluginsWillReload: utl::message<PluginsWillReload> {};
	struct PluginsDidReload: utl::message<PluginsDidReload> {};
	
}

