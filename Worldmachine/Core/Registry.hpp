#pragma once

#include "Core/Base.hpp"

#include <utl/vector.hpp>
#include <utl/hashmap.hpp>
#include <utl/function.hpp>
#include <utl/memory.hpp>
#include <utl/UUID.hpp>
#include <type_traits>
#include <string>

#include "Network/NodeImplementation.hpp"

#define WM_RegisterNode(Name)                                                       \
__attribute__((constructor)) void WM_register_##Name() {                            \
	::worldmachine::Registry::instance().registerNode<Name>(__DATE__ " " __TIME__); \
}

#define WM_RegisterPlugin(Name)                                                                                   \
WORLDMACHINE_API extern "C" char const* WM_Plugin_GetName() { return Name; }                                      \
WORLDMACHINE_API extern "C" void const* WM_Plugin_GetRegistry() { return &::worldmachine::Registry::instance(); } \
WORLDMACHINE_API extern "C" void WM_Plugin_SetImGuiData(worldmachine_ImGuiRuntimeData data) {                     \
	::worldmachine::setImGuiRuntimeData(data);                                                                    \
}                                                                                                                 \
WORLDMACHINE_API extern "C" char const* WM_Plugin_BuildTime() { return __DATE__ " " __TIME__; }

namespace worldmachine {
	
	class Registry {
		friend class Plugin;
		friend class PluginManager;
		class NodeVTable {
		public:
			utl::UUID pluginID() const { return _pluginID; }
			std::string_view buildTime() const { return _buildTime; };
			utl::unique_ref<NodeImplementation> createImplementation() const { return _create(); }
			NodeDescriptor descriptor() const { return _descriptor; };
			
		private:
			friend class Registry;
			friend class Plugin;
			utl::UUID _pluginID;
			std::string _buildTime;
			utl::function<utl::unique_ref<NodeImplementation>()> _create;
			NodeDescriptor _descriptor;
		};
		
	public:
		static Registry& instance();
		
	public:
		template <typename NodeType>
		void registerNode(char const* buildTime);
		
		utl::unique_ref<NodeImplementation> createNodeImplementation(ImplementationID, utl::UUID nodeID) const;
		NodeDescriptor createDescriptorFromID(ImplementationID) const;
		utl::vector<ImplementationID> getIDs() const;
		
		void displayEntries() const;
		
	private:
		NodeVTable const& getVTable(ImplementationID) const;
		void eraseWithPluginID(utl::UUID);
		void merge(Registry const& other);
		
	private:
		utl::hashmap<ImplementationID, NodeVTable> vtables;
	};
	
}

struct worldmachine_ImGuiRuntimeData {
	std::aligned_storage_t<128> _;
};

namespace worldmachine {
	worldmachine_ImGuiRuntimeData getImGuiRuntimeData();
	void setImGuiRuntimeData(worldmachine_ImGuiRuntimeData);

	template <typename NodeType>
	void Registry::registerNode(char const* buildTime) {
		
		NodeDescriptor desc = NodeType::staticDescriptor();
		desc.name = NodeType::staticName();
		desc.implementationID = NodeType::staticID();
		
		NodeVTable vtable;
		vtable._pluginID = {};
		vtable._buildTime = buildTime;
		vtable._create = []{
			return utl::make_unique_ref<NodeType>();
		};
		vtable._descriptor = desc;
		
		bool const success = vtables.insert({
			NodeType::staticID(),
			vtable
		}).second;
		if (!success) {
			throw std::runtime_error(utl::format("Node '{}' already exists", NodeType::staticName()));
		}
	}
	
}
