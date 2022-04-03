#include "Network.hpp"

#include <optional>
#include <span>
#include <algorithm>
#include <yaml-cpp/yaml.h>

#include "Core/Debug.hpp"
#include "Core/Registry.hpp"
#include "Core/GlobalMessenger.hpp"

#include "NodeImplementation.hpp"
#include "NetworkTraversal.hpp"

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
		desc.position.z = (float)nodes.size();
		
		Node elem = {
			.name               = desc.name,
			.category           = desc.category,
			.position           = desc.position,
			.size               = nodeSize(m_nodeParams, pinCount),
			.buildProgress      = 0.0f,
			.pinCount           = pinCount,
			.flags              = NodeFlags::none,
			.id                 = nodeID,
			.implementation     = std::move(impl),
			.pinDescriptorArray = desc.pinDescriptorArray
		};
		
		nodes.push_back(elem);
		
		std::size_t const nodeIndex = nodes.size() - 1;
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
		for (std::size_t i = 0;
			 auto& targetID: nodes.view<Node::members::id>())
		{
			if (targetID == id) {
				return i;
			}
			++i;
		}
		return -1;
	}
	
	utl::UUID NetworkBase::IDFromIndex(std::size_t nodeIndex) const {
		return nodes[nodeIndex].id;
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
		auto id = globalMessenger().register_listener([this](PluginsWillReload) {
			for (auto& impl: nodes.view<Node::members::implementation>()) {
				YAML::Emitter out;
				out << YAML::BeginMap;
				impl->serializer().serialize(out);
				out << YAML::EndMap;
				
				_storedImplementationState.push_back({
					impl->implementationID(),
					out.c_str()
				});
				impl.reset();
			}
		});
		_listenerIDs.insert(std::move(id));
		
		id = globalMessenger().register_listener([this](PluginsDidReload){
			std::size_t index = 0;
			for (auto [impl, nodeID]: nodes.view<Node::members::implementation, Node::members::id>()) {
				auto [implementationID, text] = _storedImplementationState[index++];
				impl = Registry::instance().createNodeImplementation(implementationID, nodeID);
				
				YAML::Node yamlNode = YAML::Load(text);
				impl->serializer().deserialize(yamlNode);
			}
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
		for (std::size_t nodeIndex = nodes.size() - 1;
			 auto node: utl::reverse(nodes))
		{
			auto const nodeBox = this->nodeBounds(node.position.xy, node.size);

			// test collision with pins
			if (auto const pin = pinHitTest(hitPosition,
											node.position.xy,
											node.size,
											node.pinCount))
			{
				NetworkHitResult result;
				result.type = NetworkHitResult::Type::pin;
				result.node = {
					.index = nodeIndex,
					.category = node.category
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
					.category = node.category
				};
				return result;
			}
			
			--nodeIndex;
		}
		

		// test edges
		for (std::size_t edgeIndex = 0;
			 auto& proxy: edges.view<Edge::members::proxy>())
		{
			mtl::line_segment_2D<> const edge = { proxy.begin, proxy.end };
			if (mtl::distance(edge, hitPosition) <= 2 * this->edgeParams().width) {
				NetworkHitResult result;
				result.type = NetworkHitResult::Type::edge;
				result.edge = {
					.index = edgeIndex
				};
				return result;
			}
			
			++edgeIndex;
		}
		
		// when we get to here we've hit the background
		return NetworkHitResult{
			.type = NetworkHitResult::Type::background
		};
	}
	
	std::optional<NetworkHitResult::Pin> Network::pinHitTest(mtl::float2 hitPosition,
															 mtl::float2 nodePosition,
															 mtl::float2 nodeSize,
															 PinCount<> pinCount) const
	{
		auto const testPin = [&](PinKind pinKind) -> std::optional<NetworkHitResult::Pin> {
			for (std::size_t i = 0; i < pinCount.get(pinKind); ++i) {
				auto const pin = this->pinDisk(nodePosition,
											   nodeSize,
											   pinKind,
											   i,
											   this->nodeParams());
				
				if (do_intersect(pin, hitPosition)) {
					return NetworkHitResult::Pin{
						.position = pin.origin,
						.index = i,
						.kind = pinKind
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
			if (a.nodeIndex < nodes.size() && b.nodeIndex < nodes.size()) {
				WM_Log(error, "Failed to add edge from {} to {}: {}",
					   nodes[a.nodeIndex].name,
					   nodes[b.nodeIndex].name,
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
		
		if (a.nodeIndex >= nodes.size() || b.nodeIndex >= nodes.size()) {
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
		
		auto const dataTypeX = nodes[x.nodeIndex].pinDescriptorArray.get(x.pinKind).at(x.pinIndex).dataType();
		auto const dataTypeY = nodes[y.nodeIndex].pinDescriptorArray.get(y.pinKind).at(y.pinIndex).dataType();
		
		
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
		std::optional<Edge> removedEdge;
		if (edgeToRemove) {
			removedEdge = edges[*edgeToRemove];
			edges.erase(*edgeToRemove);
		}
		
		edges.push_back({
			.beginNodeIndex = from.nodeIndex,
			.endNodeIndex   = to.nodeIndex,
			.beginPinIndex  = from.pinIndex,
			.endPinIndex    = to.pinIndex,
			.beginPinKind   = from.pinKind,
			.endPinKind     = to.pinKind,
			.proxy          = makeEdgeProxy(from, to)
		});
		
		if (hasCycles(this, to.nodeIndex)) {
			edges.pop_back();
			if (removedEdge) {
				/// If we removed an Edge to make room for this one then restore it
				edges.push_back(*removedEdge);
			}
			throw NetworkCycleError("Edge would introduce a cycle.");
		}
		
		invalidateNodesDownstream(to.nodeIndex);
	}
	
	EdgeProxy Network::makeEdgeProxy(PinIndex const& from, PinIndex const& to) {
		auto const getPinPosition = [&, this](PinIndex const& desc) {
			auto const nodePosition = nodes[desc.nodeIndex].position;
			auto const nodeSize     = nodes[desc.nodeIndex].size;
			
			return this->pinPosition(nodePosition.xy,
									 nodeSize,
									 desc.pinKind,
									 desc.pinIndex,
									 nodeParams());
		};
		return { getPinPosition(from), getPinPosition(to) };
	}
	
	void Network::removeEdge(std::size_t edgeIndex) {
		WM_BoundsCheck(edgeIndex, 0, edgeCount());
		std::size_t const nodeIndex = edges[edgeIndex].endNodeIndex;
		edges.erase(edgeIndex);
		invalidateNodesDownstream(nodeIndex);
	}
	
	void Network::removeSelectedNodes() {
		auto const selectedIDs = IDsFromIndices(Selection::indices);
		
		WM_Log("Removing nodes {}", Selection::indices);
		
		clearSelection();
		
		for (auto id: selectedIDs) {
			removeNode(id);
		}
	}
	
	void Network::removeNode(utl::UUID nodeID) {
		removeNode(indexFromID(nodeID));
	}
	
	void Network::removeNode(std::size_t nodeIndex) {
		WM_BoundsCheck(nodeIndex, 0, nodeCount());
		nodes.erase(nodeIndex);
		utl::small_vector<std::uint32_t, 24> edgesToRemove;
		
		for (std::size_t edgeIndex = 0; auto edge: edges) {
			if (edge.beginNodeIndex == nodeIndex || edge.endNodeIndex == nodeIndex) {
				edgesToRemove.push_back(utl::narrow_cast<std::uint32_t>(edgeIndex));
			}
			if (edge.beginNodeIndex > nodeIndex)
				--edge.beginNodeIndex;
			if (edge.endNodeIndex > nodeIndex)
				--edge.endNodeIndex;
			++edgeIndex;
		}
		
		// hacky way to adjust the edge indices after each removal
		std::sort(edgesToRemove.begin(), edgesToRemove.end());
		for (std::size_t i = 0; auto edgeIndex: edgesToRemove) {
			edges.erase(edgeIndex - i);
			++i;
		}
		
		// adjust z position
//		for (std::size_t i = nodeIndex; i < nodes.size(); ++i) {
//			nodes[i].position.z -= 1;
//		}
		for (auto node: nodes) {
			if (node.position.z >= nodeIndex) {
				node.position.z -= 1;
			}
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
		Selection::clearCandidates();
		
		for (std::size_t index = 0; auto [position, size]: nodes.view<Node::members::position, Node::members::size>()) {
			clearNodeFlag(index, NodeFlags::selected);
			if (do_intersect(nodeBounds(position.xy, size), rect)) {
				Selection::addCandidate(index);
			}
			++index;
		}
		
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
		
		float const zPosition = nodes[oldIndex].position.z;
		
		for (auto& pos: nodes.view<Node::members::position>()) {
			if (pos.z < zPosition) {
				pos.z += 1;
			}
		}
		
		nodes[oldIndex].position.z = 0;
	}
	
	void Network::moveSelected(mtl::float2 offset) {
		for (auto index: Selection::indices) {
			nodes[index].position.xy += offset;
		}
		
		// move edges
		for (auto selectedNodeIndex: Selection::indices) {
			for (auto edge: edges) {
				if (edge.beginNodeIndex == selectedNodeIndex) {
					edge.proxy.begin += offset;
				}
				else if (edge.endNodeIndex == selectedNodeIndex) {
					edge.proxy.end += offset;
				}
				else { /* don't care */ }
			}
		}
	}
	
	std::optional<std::size_t> Network::edgeInPin(PinIndex const& desc) const {
		for (std::size_t edgeIndex = 0; auto edge: edges) {
			if (edge.endNodeIndex == desc.nodeIndex &&
				edge.endPinIndex  == desc.pinIndex &&
				edge.endPinKind   == desc.pinKind)
			{
				return edgeIndex;
			}
			++edgeIndex;
		}
		return std::nullopt;
	}
	
	/// MARK: - Traversal and queries	
	void Network::invalidateAllNodes(BuildType type) {
		auto const roots = gatherRootNodes();
		for (auto& r: roots) {
			invalidateNodesDownstream(r, type);
		}
	}
	
	void Network::invalidateNodesDownstream(std::size_t nodeIndex, BuildType type) {
		WM_BoundsCheck(nodeIndex, 0, nodes.size());
		
		for (std::size_t const downstreamNodeIndex: NetworkTraversalView(this, nodeIndex)) {
			if (test(type & BuildType::highResolution)) {
				nodes[downstreamNodeIndex].flags &= ~NodeFlags::built;
				nodes[downstreamNodeIndex].implementation->_built = false;
			}
			if (test(type & BuildType::preview)) {
				nodes[downstreamNodeIndex].flags &= ~NodeFlags::previewBuilt;
				nodes[downstreamNodeIndex].implementation->_previewBuilt = false;
			}
		}
	}
	
	void Network::invalidateNodesDownstream(utl::UUID nodeID, BuildType type) {
		invalidateNodesDownstream(indexFromID(nodeID), type);
	}
	
	utl::small_vector<std::size_t> Network::gatherLeafNodes() {
		auto condition = [this](auto nodeIndex) {
			for (auto beginNodeIndex: edges.view<Edge::members::beginNodeIndex>()) {
				if (nodeIndex == beginNodeIndex) {
					/* if any edge comes from nodes[nodeIndex] then we are not a leaf */
					return false;
				}
			}
			return true;
		};
		return gatherNodesImpl(condition);
	}
	
	utl::small_vector<std::size_t> Network::gatherRootNodes() {
		auto condition = [this](auto nodeIndex) {
			for (auto endNodeIndex: edges.view<Edge::members::endNodeIndex>()) {
				if (nodeIndex == endNodeIndex) {
					/* if any edge ends in nodes[nodeIndex] then we are not a root */
					return false;
				}
			}
			return true;
		};
		
		return gatherNodesImpl(condition);
	}
	
	utl::small_vector<std::size_t> Network::gatherNodesImpl(auto&& cond) {
		utl::small_vector<std::size_t> result;
		for (std::size_t nodeIndex: utl::iota<std::size_t>(0, nodes.size())) {
			if (cond(nodeIndex)) {
				result.push_back(nodeIndex);
			}
		}
		return result;
	}
	
	NodeEdges Network::collectNodeEdges(std::size_t nodeIndex) const {
		WM_BoundsCheck(nodeIndex, 0, nodes.size());
		auto const& pinDescArray = nodes[nodeIndex].pinDescriptorArray;
		
		NodeEdges result;
		result.inputEdges.resize(pinDescArray.input.size());
		result.maskInputEdges.resize(pinDescArray.maskInput.size());
		
		for (std::size_t i = 0; i < pinDescArray.input.size(); ++i) {
			result.inputEdges[i].mandatory = pinDescArray.input[i].mandatory();
		}
		for (std::size_t i = 0; i < pinDescArray.maskInput.size(); ++i) {
			result.maskInputEdges[i].mandatory = pinDescArray.maskInput[i].mandatory();
		}
		
		auto inputEdgesFor = [&](PinKind k) -> utl::vector<NodeEdges::EdgeRepresentation>& {
			if (k == PinKind::input) {
				return result.inputEdges;
			}
			else if (k == PinKind::maskInput) {
				return result.maskInputEdges;
			}
			else { WM_DebugBreak(); }
		};
		
		for (auto edge: edges) {
			if (edge.endNodeIndex != nodeIndex) {
				continue;
			}
			if (edge.endPinKind != PinKind::input && edge.endPinKind != PinKind::maskInput) {
				continue;
			}
			inputEdgesFor(edge.endPinKind)[edge.endPinIndex] = NodeEdges::EdgeRepresentation{
				edge,
				.present        = true,
				.mandatory      = nodes[edge.beginNodeIndex].pinDescriptorArray.get(edge.beginPinKind)[edge.beginPinIndex].mandatory()
			};
		}
		
		return result;
	}
	
	bool Network::allMandatoryUpstreamNodesConnected(utl::UUID id) const {
		return allMandatoryUpstreamNodesConnected(indexFromID(id));
	}
	
	bool Network::allMandatoryUpstreamNodesConnected(std::size_t nodeIndex) const {
		for (std::size_t const upstreamIndex: utl::reverse(NetworkTraversalView(this, nodeIndex))) {
			auto const nodeEdges = collectNodeEdges(upstreamIndex);
			if (!nodeEdges.dependenciesConnected()) {
				return false;
			}
		}
		return true;
	}
	
	
}
