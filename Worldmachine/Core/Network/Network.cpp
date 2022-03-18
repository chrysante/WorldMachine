#include "Network.hpp"

#include <optional>
#include <span>
#include <algorithm>
#include <yaml-cpp/yaml.h>

#include "Core/Debug.hpp"
#include "Core/Registry.hpp"
#include "Core/GlobalMessenger.hpp"

#include "NodeImplementation.hpp"

using namespace mtl::short_types;

namespace worldmachine {
	
	/// MARK: - NodeCollection
	NodeCollection::NodeCollection()
	{
		
	}
	
	std::size_t NodeCollection::addNode(NodeDescriptor desc) {
		auto const pinCount = calculatePinCount(desc);
		
		auto const nodeID = utl::UUID::generate();
		utl::unique_ref<NodeImplementation> impl = Registry::instance().createNodeImplementation(desc.implementationID, nodeID);
		desc.position.z = nodes().size();
		
		nodes().push_back(desc.name,
						  desc.category,
						  desc.position,
						  nodeSize(m_nodeParams, pinCount),
						  0.0f /* buildProgress */,
						  pinCount,
						  NodeFlags::none,
						  nodeID,
						  std::move(impl),
						  desc.pinDescriptorArray);
		

		std::size_t const nodeIndex = nodes().size() - 1;
		return nodeIndex;
	}
	
	mtl::float2 NodeCollection::pinPosition(mtl::float2 nodePosition,
											mtl::float2 nodeSize,
											PinKind pinKind,
											std::size_t pinIndex,
											NodeParameters const& nodeParams)
	{
		auto const nodeBounds = NodeCollection::nodeBounds(nodePosition, nodeSize);
		
		switch (pinKind) {
			case PinKind::input: {
				auto const pX = nodeBounds.origin.x;
				
				auto const pY = nodeBounds.origin.y + nodeBounds.extend.y - nodeParams.cornerRadius - (pinIndex + 0.5f) * nodeParams.pinSpacing;
				
				return { pX, pY };
			}
			case PinKind::output: {
				auto const pX = nodeBounds.origin.x + nodeBounds.extend.x;
				
				auto const pY = nodeBounds.origin.y + nodeBounds.extend.y - nodeParams.cornerRadius - (pinIndex + 0.5f) * nodeParams.pinSpacing;
				
				return { pX, pY };
			}
			case PinKind::parameterInput: {
				auto const pX = nodeBounds.origin.x + nodeParams.cornerRadius + (pinIndex + 0.5f) * nodeParams.parameterPinSpacing;
				
				auto const pY = nodeBounds.origin.y + nodeBounds.extend.y;
				
				return { pX, pY };
			}
			case PinKind::maskInput: {
				auto const pX = nodeBounds.origin.x + nodeParams.cornerRadius + (pinIndex + 0.5f) * nodeParams.pinSpacing;
				
				auto const pY = nodeBounds.origin.y;
				
				return { pX, pY };
			}
			default: throw;
		}
	}
	
	mtl::disk<> NodeCollection::pinDisk(mtl::float2 nodePosition,
										mtl::float2 nodeSize,
										PinKind pinKind,
										std::size_t pinIndex,
										NodeParameters const& nodeParams)
	{
		return {
			pinPosition(nodePosition, nodeSize, pinKind, pinIndex, nodeParams),
			(pinKind == PinKind::parameterInput ?
			 nodeParams.parameterPinSpacing :
			 nodeParams.pinSpacing) / 2
		};
	}
	
	/// MARK: - NetworkBase
	long NetworkBase::indexFromID(utl::UUID id) const {
		auto const index = nodes().find_if<Node::ID>([&, i = std::size_t{ 0 }](auto targetID) mutable -> std::optional<std::size_t> {
			if (targetID == id) {
				return i;
			}
			else {
				++i;
				return std::nullopt;
			}
		});
		return index ? *index : -1;
	}
	
	utl::UUID NetworkBase::IDFromIndex(std::size_t nodeIndex) const {
		return nodes().get<Node::ID>(nodeIndex);
	}
	
	utl::small_vector<std::size_t> NetworkBase::indicesFromIDs(std::span<utl::UUID const> ids) const {
		utl::small_vector<std::size_t> result;
		result.reserve(ids.size());
		for (auto id: ids) {
			auto const index = indexFromID(id);
			WM_Assert(index >= 0);
			result.push_back(index);
		}
		return result;
	}
	
	utl::small_vector<utl::UUID> NetworkBase::IDsFromIndices(std::span<std::size_t const> nodeIndices) const {
		utl::small_vector<utl::UUID> result;
		result.reserve(nodeIndices.size());
		for (auto nodeIndex: nodeIndices) {
			result.push_back(IDFromIndex(nodeIndex));
		}
		return result;
	}
	
	/// MARK: - Network
	Network::Network() {
		auto id = globalMessenger().register_listener([this](PluginsWillReload){
			nodes().for_each<Node::Implementation>([&](Node::Implementation& impl) {
				YAML::Emitter out;
				out << YAML::BeginMap;
				impl->serializer().serialize(out);
				out << YAML::EndMap;
				
				_storedImplementationState.push_back({
					impl->implementationID(),
					out.c_str()
				});
				impl.reset();
			});
		});
		_listenerIDs.insert(std::move(id));
		
		id = globalMessenger().register_listener([this](PluginsDidReload){
			std::size_t index = 0;
			nodes().for_each<Node::Implementation, Node::ID>([&](Node::Implementation& impl,
																 auto nodeID) {
				auto [implementationID, text] = _storedImplementationState[index++];
				impl = Registry::instance().createNodeImplementation(implementationID, nodeID);
				
				YAML::Node yamlNode = YAML::Load(text);
				impl->serializer().deserialize(yamlNode);
			});
			invalidateAllNodes();
		});
		_listenerIDs.insert(std::move(id));
	}

	utl::unique_ref<Network> Network::create() {
		return utl::unique_ref<Network>(new Network());
	}
	
	/// MARK: - Modification and interaction
	NetworkHitResult Network::hitTest(mtl::float2 hitPosition) const {
		// test nodes
		if (auto result = this->nodes().find_if<utl::backward,
												Node::Position,
												Node::Size,
												NodeCategory,
												PinCount<>>([&](auto nodeIndex,
																auto const& nodePosition,
																auto const& nodeSize,
																auto const& nodeCategory,
																auto const& nodePinCount) -> std::optional<NetworkHitResult> {
			auto const nodeBox = this->nodeBounds(nodePosition.xy, nodeSize);

			// test collision with pins
			if (auto const pin = pinHitTest(hitPosition,
											nodePosition.xy,
											nodeSize,
											nodePinCount)) {
				NetworkHitResult result;
				result.type = NetworkHitResult::Type::pin;
				result.node = {
					.index = nodeIndex,
					.category = nodeCategory
				};
				result.pin = *pin;
				return result;
			}


			// test collision with node
			if (do_intersect(nodeBox, hitPosition)) {
				NetworkHitResult result;
				result.type = NetworkHitResult::Type::node;
				result.node = {
					.index = nodeIndex,
					.category = nodeCategory
				};
				return result;
			}

			return std::nullopt;
		})) {
			return *result;
		}
		

		// test edges
		if (auto result = this->edges().find_if<Edge::Proxy>([&](auto edgeIndex,
																 Edge::Proxy const& proxy) -> std::optional<NetworkHitResult> {
			mtl::line_segment_2D<> const edge = { proxy.begin, proxy.end };
			if (mtl::distance(edge, hitPosition) <= 2 * this->edgeParams().width) {
				NetworkHitResult result;
				result.type = NetworkHitResult::Type::edge;
				result.edge = {
					.index = edgeIndex
				};
				return result;
			}
			
			return std::nullopt;
		})) {
			return *result;
		}

		
		// hit background
		return NetworkHitResult{
			.type = NetworkHitResult::Type::background
		};
	}
	
	std::optional<NetworkHitResult::Pin> Network::pinHitTest(mtl::float2 hitPosition,
															 mtl::float2 nodePosition,
															 mtl::float2 nodeSize,
															 PinCount<> ifCount) const
	{
		auto const testPin = [&](PinKind ifKind) -> std::optional<NetworkHitResult::Pin> {
			for (std::size_t i = 0; i < ifCount.get(ifKind); ++i) {
				auto const pin = this->pinDisk(nodePosition,
														   nodeSize,
														   ifKind,
														   i,
														   this->nodeParams());
				
				if (do_intersect(pin, hitPosition)) {
					return NetworkHitResult::Pin{
						.index = i,
						.position = pin.origin,
						.kind = ifKind
					};
				}
			}
			return std::nullopt;
		};
		
		if (auto const result = testPin(PinKind::input)) {
			return result;
		}
		else if (auto const result = testPin(PinKind::output)) {
			return result;
		}
		else if (auto const result = testPin(PinKind::parameterInput)) {
			return result;
		}
		else if (auto const result = testPin(PinKind::maskInput)) {
			return result;
		}
		else {
			return std::nullopt;
		}
	}
	
	void Network::tryAddEdge(PinIndex const& a,
							 PinIndex const& b)
	{
		try {
			addEdge(a, b);
		}
		catch (std::exception const& e) {
			if (a.nodeIndex < nodes().size() && b.nodeIndex < nodes().size()) {
				WM_Log(error, "Failed to add edge from {} to {}: {}",
					   nodes().get<Node::Name>(a.nodeIndex),
					   nodes().get<Node::Name>(b.nodeIndex),
					   e.what());
			}
			else {
				WM_Log(error, "Failed to add edge from node {} to node {}", a.nodeIndex, b.nodeIndex);
			}
		}
	}
	
	
	void Network::addEdge(PinIndex const& a,
						  PinIndex const& b)
	{
		if (!isCompatibleForImageNode(a.pinKind, b.pinKind)) {
			throw std::runtime_error(utl::format("{} and {} is incompatible.", a.pinKind, b.pinKind));
		}
		if (a.nodeIndex == b.nodeIndex) {
			throw std::runtime_error("Source and target are the same node");
		}
		
		if (a.nodeIndex >= nodes().size() || b.nodeIndex >= nodes().size()) {
			throw std::runtime_error("Node Index does not exist");
		}
		
		// normalize edge direction
		auto const [/* output -> */ x, /* -> input */ y] = [&]{
			if (a.pinKind == PinKind::output) {
				WM_Assert(b.pinKind == PinKind::input || b.pinKind == PinKind::maskInput,
						  "b must be either input or maskInput   \n"
						  "Error should have been handled by isCompatibleForImageNode(...)");
				return std::pair{ a, b };
			}
			else {
				WM_Assert(a.pinKind == PinKind::input || a.pinKind == PinKind::maskInput,
						  "a must be either input or maskInput   \n"
						  "Error should have been handled by isCompatibleForImageNode(...)");
				return std::pair{ b, a };
			}
		}();
		
		auto const dataTypeX = nodes().get<NodePinDescriptorArray>(x.nodeIndex).get(x.pinKind).at(x.pinIndex).dataType();
		auto const dataTypeY = nodes().get<NodePinDescriptorArray>(y.nodeIndex).get(y.pinKind).at(y.pinIndex).dataType();
		
		
		WM_Assert(std::popcount(utl::to_underlying(dataTypeX)) < 2, "More than one data type flag is set in output");
		if (!(dataTypeX & dataTypeY)) {
			throw std::runtime_error(utl::format("Incompatible data types \"{}\" and \"{}\".",
												 dataTypeX, dataTypeY));
		}
		
		addEdgeImpl(x, y, edgeInPin(y));
	}
	
	// private
	void Network::addEdgeImpl(PinIndex const& from, PinIndex const& to,
							  std::optional<std::size_t> edgeToRemove) {
		std::optional<EdgeType> removedEdge;
		if (edgeToRemove) {
			removedEdge = edges()[*edgeToRemove];
			edges().erase(*edgeToRemove);
		}
		this->edges().push_back(from.nodeIndex,
								to.nodeIndex,
								from.pinIndex,
								to.pinIndex,
								from.pinKind,
								to.pinKind,
								makeEdgeProxy(from, to));
		try {
			traverseDownstreamNodes(to.nodeIndex, [](auto){});
		}
		catch (NetworkCycleError const& e) {
			this->edges().pop_back();
			if (removedEdge) {
				this->edges().push_back(*removedEdge);
			}
			throw NetworkCycleError("Edge would introduce a cycle.");
		}
		invalidateNodesDownstream(to.nodeIndex);
	}
	
	EdgeProxy Network::makeEdgeProxy(PinIndex const& from, PinIndex const& to) {
		auto const getPinPosition = [&, this](PinIndex const& desc) {
			auto const nodePosition = this->nodes().get<Node::Position>(desc.nodeIndex);
			auto const nodeSize     = this->nodes().get<Node::Size>(desc.nodeIndex);
			
			return this->pinPosition(nodePosition.xy,
									 nodeSize,
									 desc.pinKind,
									 desc.pinIndex,
									 this->nodeParams());
		};
		return { getPinPosition(from), getPinPosition(to) };
	}
	
	void Network::removeEdge(std::size_t edgeIndex) {
		WM_BoundsCheck(edgeIndex, 0, edgeCount());
		std::size_t const nodeIndex = edges().get<Edge::EndNodeIndex>(edgeIndex);
		edges().erase(edgeIndex);
		invalidateNodesDownstream(nodeIndex);
	}
	
	void Network::removeSelectedNodes() {
		auto const selectedIDs = IDsFromIndices(Selection::indices);
		
		for (auto id: selectedIDs) {
			removeNode(id);
		}
		clearSelection();
	}
	
	void Network::removeNode(utl::UUID nodeID) {
		removeNode(indexFromID(nodeID));
	}
	
	void Network::removeNode(std::size_t nodeIndex) {
		WM_BoundsCheck(nodeIndex, 0, nodeCount());
		nodes().erase(nodeIndex);
		utl::small_vector<std::uint32_t, 24> edgesToRemove;
		edges().for_each<Edge::BeginNodeIndex, Edge::EndNodeIndex>([&](std::uint32_t edgeIndex, auto& a, auto& b) {
			if (a == nodeIndex || b == nodeIndex) {
				edgesToRemove.push_back(edgeIndex);
			}
			if (a > nodeIndex)
				--a;
			if (b > nodeIndex)
				--b;
		});
		// hacky way to adjust the edge indices after each removal
		std::sort(edgesToRemove.begin(), edgesToRemove.end());
		for (std::size_t i = 0; auto edgeIndex: edgesToRemove) {
			edges().erase(edgeIndex - i);
			++i;
		}
	}
	
	void Network::selectNode(std::size_t index) {
		WM_BoundsCheck(index, 0, this->nodeCount());
		WM_Expect(!this->isSelected(index), "Node is already selected");
		
		this->Selection::add(index);
		this->setNodeFlag(index, NodeFlags::selected);
	}
	
	void Network::unselectNode(utl::UUID nodeID) {
		auto const index = indexFromID(nodeID);
		WM_Assert(index != -1, "ID not present in Network");
		unselectNode(index);
	}
	
	void Network::unselectNode(std::size_t index) {
		WM_BoundsCheck(index, 0, this->nodeCount());
		WM_Expect(this->isSelected(index), "Node is not selected");
		
		this->Selection::remove(index);
		this->clearNodeFlag(index, NodeFlags::selected);
	}
	
	bool Network::toggleSelectNode(std::size_t index) {
		WM_BoundsCheck(index, 0, this->nodeCount());
		
		if (this->Selection::tryRemove(index)) {
			this->clearNodeFlag(index, NodeFlags::selected);
			return false;
		}
		else {
			this->Selection::add(index);
			this->setNodeFlag(index, NodeFlags::selected);
			return true;
		}
	}
	
	void Network::clearSelection() {
		for (auto i: this->Selection::indices) {
			this->clearNodeFlag(i, NodeFlags::selected);
		}
		this->Selection::clear();
	}
	
	void Network::setRectangleSelection(mtl::rectangle<float> rect, SelectOperation op) {
		this->Selection::clearCandidates();
		
		this->nodes().for_each<Node::Position, Node::Size>([&](auto index, auto position, auto size) {
			this->clearNodeFlag(index, NodeFlags::selected);
			if (do_intersect(this->nodeBounds(position.xy, size), rect)) {
				this->Selection::addCandidate(index);
			}
		});
		
		for (auto i: this->Selection::indices) {
			this->setNodeFlag(i, NodeFlags::selected);
		}
		
		for (auto i: this->Selection::candidates) {
			switch (op) {
				case SelectOperation::setUnion: {
					this->setNodeFlag(i, NodeFlags::selected);
					break;
				}
				case SelectOperation::setDifference: {
					this->clearNodeFlag(i, NodeFlags::selected);
					break;
				}
				case SelectOperation::setIntersection: {
					this->clearNodeFlag(i, NodeFlags::selected);
					break;
				}
				case SelectOperation::setSymmetricDifference: {
					this->toggleNodeFlag(i, NodeFlags::selected);
					break;
				}
			}
			
		}
	}
	
	void Network::applyRectangleSelection(SelectOperation op) {
		switch (op) {
			case SelectOperation::setUnion:
				this->Selection::indices = utl::set_union(this->Selection::indices, this->Selection::candidates);
				break;
				
			case SelectOperation::setDifference:
				this->Selection::indices = utl::set_difference(this->Selection::indices, this->Selection::candidates);
				break;
				
			case SelectOperation::setIntersection:
				this->Selection::indices = utl::set_intersection(this->Selection::indices, this->Selection::candidates);
				break;
				
			case SelectOperation::setSymmetricDifference:
				this->Selection::indices = utl::set_symmetric_difference(this->Selection::indices, this->Selection::candidates);
				break;
		}
		
		this->Selection::candidates.clear();
	}
	
	void Network::moveToTop(std::size_t oldIndex) {
		WM_BoundsCheck(oldIndex, 0, this->nodeCount());
		
		float const zPosition = nodes().get<Node::Position>(oldIndex).z;
		
		nodes().for_each<Node::Position>([&](float3& pos) {
			if (pos.z < zPosition) {
				pos.z += 1;
			}
		});
		
		nodes().get<Node::Position>(oldIndex).z = 0;
	}
	
	void Network::moveSelected(mtl::float2 offset) {
		utl::for_each(this->Selection::indices, [&](auto index) {
			this->nodes().get<Node::Position>(index).xy += offset;
		});
		
		// move edges
		utl::for_each(this->Selection::indices, [&](auto selectedNodeIndex) {
			this->edges().for_each<Edge::BeginNodeIndex, Edge::EndNodeIndex, Edge::Proxy>([&](auto beginIndex,
																					  auto endIndex,
																					  auto& proxy) {
				if (beginIndex == selectedNodeIndex) {
					proxy.begin += offset;
				}
				if (endIndex == selectedNodeIndex) {
					proxy.end += offset;
				}
			});
		});
	}
	
	std::optional<std::size_t> Network::edgeInPin(PinIndex const& desc) const {
		return this->edges().find_if<Edge::EndNodeIndex,
									 Edge::EndPinIndex,
									 Edge::EndPinKind>([&](auto edgeIndex,
														   auto endNodeIndex,
														   auto endPinIndex,
														   auto endPinKind) -> std::optional<std::size_t> {
			if (endNodeIndex      == desc.nodeIndex &&
				endPinIndex == desc.pinIndex &&
				endPinKind  == desc.pinKind) {
				return edgeIndex;
			}
			else {
				return std::nullopt;
			}
		});
	}
	
	/// MARK: - Traversal and queries	
	void Network::invalidateAllNodes(BuildType type) {
		auto const roots = gatherRootNodes();
		for (auto& r: roots) {
			invalidateNodesDownstream(r, type);
		}
	}
	
	void Network::invalidateNodesDownstream(std::size_t nodeIndex, BuildType type) {
		WM_BoundsCheck(nodeIndex, 0, nodes().size());
		
		traverseDownstreamNodes(nodeIndex, [&, this](std::size_t downstreamNodeIndex) {
			WM_Log(debug, "Invalidating '{}'", nodes().get<Node::Name>(downstreamNodeIndex));
			if (test(type & BuildType::highResolution)) {
				nodes().get<Node::Flags>(downstreamNodeIndex) &= ~NodeFlags::built;
				nodes().get<Node::Implementation>(downstreamNodeIndex)->_built = false;
			}
			if (test(type & BuildType::preview)) {
				nodes().get<Node::Flags>(downstreamNodeIndex) &= ~NodeFlags::previewBuilt;
				nodes().get<Node::Implementation>(downstreamNodeIndex)->_previewBuilt = false;
			}
		});
	}
	
	void Network::invalidateNodesDownstream(utl::UUID nodeID, BuildType type) {
		invalidateNodesDownstream(indexFromID(nodeID), type);
	}
	
	utl::small_vector<std::size_t> Network::gatherLeaveNodes() {
		auto cond = [this](auto nodeIndex) {
			return !edges().find_if<Edge::BeginNodeIndex>([nodeIndex](auto beginNodeIndex) {
				return nodeIndex == beginNodeIndex;
			});
		};
		return _gatherNodesImpl(cond);
	}
	
	utl::small_vector<std::size_t> Network::gatherRootNodes() {
		auto cond = [this](auto nodeIndex) {
			return !edges().find_if<Edge::EndNodeIndex>([nodeIndex](auto endNodeIndex) {
				return nodeIndex == endNodeIndex;
			});
		};
		
		return _gatherNodesImpl(cond);
	}
	
	utl::small_vector<std::size_t> Network::_gatherNodesImpl(auto&& cond) {
		utl::small_vector<std::size_t> result;
		for (std::size_t nodeIndex: utl::iota<std::size_t>(0, nodes().size())) {
			if (cond(nodeIndex)) {
				result.push_back(nodeIndex);
			}
		}
		return result;
	}
	
	NodeEdges Network::collectNodeEdges(std::size_t nodeIndex) const {
		WM_BoundsCheck(nodeIndex, 0, nodes().size());
		auto const& pinDescArray = nodes().get<NodePinDescriptorArray>(nodeIndex);
		
		NodeEdges result;
		result.inputEdges.resize(pinDescArray.input.size());
		result.maskInputEdges.resize(pinDescArray.maskInput.size());
		
		for (std::size_t i = 0; i < pinDescArray.input.size(); ++i) {
			result.inputEdges[i].mandatory = pinDescArray.input[i].mandatory();
		}
		for (std::size_t i = 0; i < pinDescArray.maskInput.size(); ++i) {
			result.maskInputEdges[i].mandatory = pinDescArray.maskInput[i].mandatory();
		}
		
		
		edges().for_each<Edge::BeginNodeIndex, Edge::BeginPinIndex,
						 Edge::BeginPinKind,
						 Edge::EndNodeIndex, Edge::EndPinIndex,
						 Edge::EndPinKind>([&](std::size_t beginNodeIndex,
													 std::size_t beginPinIndex,
													 PinKind beginPinKind,
													 std::size_t endNodeIndex,
													 std::size_t endPinIndex,
													 PinKind endPinKind) {
							 if (endNodeIndex != nodeIndex) {
								 return;
							 }
							 auto const pushBack = [&](auto& container) {
								 container[endPinIndex]   = NodeEdges::EdgeRepresentation{
									 .beginNodeIndex      = beginNodeIndex,
									 .beginPinIndex       = beginPinIndex,
									 .beginPinKind        = beginPinKind,
									 .endNodeIndex        = endNodeIndex,
									 .endPinIndex         = endPinIndex,
									 .endPinKind          = endPinKind,
									 .present             = true,
									 .mandatory           = nodes().get<NodePinDescriptorArray>(beginNodeIndex).get(beginPinKind)[beginPinIndex].mandatory()
								 };
							 };
							 switch (endPinKind) {
								 case PinKind::input:
									 pushBack(result.inputEdges);
									 break;
								 case PinKind::maskInput:
									 pushBack(result.maskInputEdges);
									 break;
								 default:
									 break;
							 }
						 });
		
		return result;
	}
	
	bool Network::allMandatoryUpstreamNodesConnected(utl::UUID id) const {
		return allMandatoryUpstreamNodesConnected(indexFromID(id));
	}
	
	bool Network::allMandatoryUpstreamNodesConnected(std::size_t nodeIndex) const {
		struct DummyException{};
		try {
			traverseUpstreamNodes(nodeIndex, [this](std::size_t upstreamIndex) {
				auto const nodeEdges = collectNodeEdges(upstreamIndex);
				if (!nodeEdges.dependenciesConnected()) {
					throw DummyException{};
				}
			});
			return true;
		}
		catch (DummyException) {
			return false;
		}
	}
	
	
}
