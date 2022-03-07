#include "Registry.hpp"

#include <utl/format.hpp>
#include <exception>
#include <imgui/imgui.h>

#include "Framework/ImGuiExt.hpp"

#include "Core/Network/NetworkSerialize.hpp"
#include "Core/Network/Network.hpp"

namespace worldmachine {
	
	Registry& Registry::instance() {
		static Registry registry;
		return registry;
	}
	
	utl::unique_ref<NodeImplementation> Registry::createNodeImplementation(ImplementationID id, utl::UUID nodeID) const {
		auto result = getVTable(id).createImplementation();
		result->_nodeID = nodeID;
		result->dynamicInit();
		return result;
	}

	NodeDescriptor Registry::createDescriptorFromID(ImplementationID id) const {
		return getVTable(id).descriptor();
	}
	
	utl::vector<ImplementationID> Registry::getIDs() const {
		utl::vector<ImplementationID> result;
		result.reserve(vtables.size());
		for (auto& [id, _]: vtables) {
			result.push_back(id);
		}
		return result;
	}

	Registry::NodeVTable const& Registry::getVTable(ImplementationID id) const {
		auto itr = vtables.find(id);
		if (itr == vtables.end()) {
			throw std::runtime_error(utl::format("Can't find id '{}'", id));
		}
		return itr->second;
	}
	 
	void Registry::eraseWithPluginID(utl::UUID id) {
		for (auto itr = vtables.begin(); itr != vtables.end();) {
			if (itr->second.pluginID() == id) {
				itr = vtables.erase(itr);
			}
			else {
				++itr;
			}
		}
	}
	
	void Registry::merge(Registry const& other) {
		for (auto& [id, vtable]: other.vtables) {
			this->vtables[id] = vtable;
		}
	}

	void Registry::displayEntries() const {
		if (ImGui::BeginTable("Registry Entries", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable)) {
			ImGui::TableSetupColumn("Node");
			ImGui::TableSetupColumn("Implementation ID");
			ImGui::TableSetupColumn("Plugin ID");
			ImGui::TableSetupColumn("Build Time");
			ImGui::TableSetupColumn("VTable Pointer");
			ImGui::TableHeadersRow();
			for (auto& [implementationID, vtable]: vtables) {
				ImGui::TableNextRow();
				auto desc = vtable.descriptor();
				auto impl = vtable.createImplementation();
				void* implVTablePtr;
				std::memcpy(&implVTablePtr, (void*)impl.get(), sizeof(void*));
				ImGui::TableSetColumnIndex(0);
				ImGui::Textf("{}", desc.name);
				ImGui::TableSetColumnIndex(1);
				ImGui::Textf("{}", implementationID);
				ImGui::TableSetColumnIndex(2);
				ImGui::Textf("{}", vtable.pluginID());
				ImGui::TableSetColumnIndex(3);
				ImGui::Textf("{}", vtable.buildTime());
				ImGui::TableSetColumnIndex(4);
				ImGui::Textf("{}", implVTablePtr);
			}
			ImGui::EndTable();
		}
	}

}


namespace worldmachine {
	
	struct ImGuiRuntimeDataImpl {
		ImGuiContext* context;
		ImGuiMemAllocFunc allocFn;
		ImGuiMemFreeFunc freeFn;
		void* userdata;
	};
	
	void setImGuiRuntimeData(worldmachine_ImGuiRuntimeData blob) {
		ImGuiRuntimeDataImpl data;
		std::memcpy(&data, &blob, sizeof data);
		ImGui::SetCurrentContext(data.context);
		ImGui::SetAllocatorFunctions(data.allocFn,
									 data.freeFn,
									 data.userdata);
	}
	
	worldmachine_ImGuiRuntimeData getImGuiRuntimeData() {
		ImGuiRuntimeDataImpl data;
		data.context = ImGui::GetCurrentContext();
		ImGui::GetAllocatorFunctions(&data.allocFn,
									 &data.freeFn,
									 &data.userdata);
		worldmachine_ImGuiRuntimeData blob;
		std::memcpy(&blob, &data, sizeof data);
		return blob;
	}
	
}
