#define UTL_DEFER_MACROS

#include <utl/scope_guard.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>

#include "Core/Registry.hpp"
#include "NetworkSerialize.hpp"
#include "Network.hpp"
#include "NodeImplementation.hpp"
#include "NodeSerializer.hpp"

namespace mtl {

	template <typename T, std::size_t Size, mtl::vector_options O>
	YAML::Emitter& operator<<(YAML::Emitter& out, mtl::vector<T, Size, O> const& v) {
		out << YAML::Flow << YAML::BeginSeq;
		for (auto i: v) {
			out << i;
		}
		return out << YAML::EndSeq;
	}

}

template <typename T, std::size_t Size, mtl::vector_options O>
struct YAML::convert<mtl::vector<T, Size, O>> {
	static Node encode(mtl::vector<T, Size, O> const& v) {
		Node node;
		for (auto i: v) {
			node.push_back(i);
		}
		return node;
	}
	
	static bool decode(Node const& node, mtl::vector<T, Size, O>& v) {
		if (!node.IsSequence() || node.size() != Size) { return false; }
		
		UTL_WITH_INDEX_SEQUENCE((I, Size), {
			v = { node[I].as<T>()... };
		});
		return true;
	}
};

namespace worldmachine {
	
	static void serializeNode(YAML::Emitter& out,
							  Node::const_reference node) {
		out << YAML::BeginMap;
		utl_defer { out << YAML::EndMap; };
		
		out << YAML::Key << "ImplementationID" << YAML::Value << node.implementation->implementationID().value();
		out << YAML::Key << "Name" << YAML::Value << node.name;
		out << YAML::Key << "Position" << YAML::Value << node.position;
		auto const* impl = node.implementation.get();
		impl->serializer().serialize(out);
	}
	
	static void serializeEdge(YAML::Emitter& out,
							  Edge::const_reference edge) {
		out << YAML::BeginMap;
		utl_defer { out << YAML::EndMap; };
		
		out << YAML::Key << "BeginNodeIndex" << YAML::Value << edge.beginNodeIndex;
		out << YAML::Key << "EndNodeIndex"   << YAML::Value << edge.endNodeIndex;
		out << YAML::Key << "BeginPinIndex"  << YAML::Value << edge.beginPinIndex;
		out << YAML::Key << "EndPinIndex"    << YAML::Value << edge.endPinIndex;
		out << YAML::Key << "BeginPinKind"   << YAML::Value << utl::to_underlying(edge.beginPinKind);
		out << YAML::Key << "EndPinKind"     << YAML::Value << utl::to_underlying(edge.endPinKind);
	}
	
	std::string serializeNetwork(Network const& network) {
		WM_Log("Serializing Network with {} nodes and {} edges", network.nodeCount(), network.edgeCount());
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			utl_defer { out << YAML::EndMap; };
			
			out << YAML::Key << "WorldMachine File" << YAML::Value << "ignore";
			
			out << YAML::Key << "Nodes" << YAML::BeginSeq;
			for (auto node: network.nodes) {
				serializeNode(out, node);
			}
			out << YAML::EndSeq;
			
			out << YAML::Key << "Edges" << YAML::BeginSeq;
			for (auto edge: network.edges) {
				serializeEdge(out, edge);
			}
			out << YAML::EndSeq;
		}
		return out.c_str();
	}
	
	static bool deserializeNetworkImpl(Network& network, std::string_view text);
	
	bool deserializeNetwork(Network& network, std::string_view text) {
		try {
			auto const result = deserializeNetworkImpl(network, text);
			WM_Log("Deserialized Network with {} nodes and {} edges", network.nodeCount(), network.edgeCount());
			return result;
		} catch (std::exception const& e) {
			WM_Log(error, "Deserialization failed: {}", e.what());
			return false;
		}
	}
	

	static bool deserializeNetworkImpl(Network& network, std::string_view text) {
		network.clear();
		YAML::Node data = YAML::Load(text.data());
		if (!data["WorldMachine File"]) {
			return false;
		}
		
		auto getValue = [&](auto&& s, char const* name){
			auto elem = s[name];
			if (!elem) {
				throw std::runtime_error("File corrupted.");
			}
			return elem;
		};
		
		// nodes
		if (auto nodes = data["Nodes"]) {
			network.nodes.reserve(nodes.size());
			for (auto yamlNode: nodes) {
				ImplementationID const implementationID{ getValue(yamlNode, "ImplementationID").as<ImplementationID::ValueType>() };
				std::string const name     = getValue(yamlNode, "Name").as<std::string>();
				mtl::float3 const position = getValue(yamlNode, "Position").as<mtl::float3>();
				// we have gathered enough information to create the node
				try {
					auto& registry = Registry::instance();
					auto desc = registry.createDescriptorFromID(implementationID);
				
					desc.name = name;
					desc.position = position;
					std::size_t const nodeIndex = network.addNode(desc);
					network.nodes[nodeIndex].implementation->serializer().deserialize(yamlNode);
				}
				catch (std::exception const& e) {
					WM_Log(error, "Failed to create Node \"{}\": {}", name, e.what());
				}
			}
		}
		
		
		// edges
		if (auto edges = data["Edges"]) {
			network.edges.reserve(edges.size());
			for (auto edge: edges) {
				auto const BeginNodeIndex      = getValue(edge, "BeginNodeIndex").as<std::size_t>();
				auto const EndNodeIndex        = getValue(edge, "EndNodeIndex").as<std::size_t>();
				auto const BeginPinIndex = getValue(edge, "BeginPinIndex").as<std::size_t>();
				auto const EndPinIndex   = getValue(edge, "EndPinIndex").as<std::size_t>();
				auto const BeginPinKind  = PinKind(getValue(edge, "BeginPinKind").as<std::underlying_type_t<PinKind>>());
				auto const EndPinKind    = PinKind(getValue(edge, "EndPinKind").as<std::underlying_type_t<PinKind>>());
				
				network.tryAddEdge({ BeginNodeIndex, BeginPinIndex, BeginPinKind },
								{ EndNodeIndex, EndPinIndex, EndPinKind });
			}
		}
		
		
		return true;
	}
	
}

