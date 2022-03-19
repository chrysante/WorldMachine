#include "BuildSystem.hpp"
#include "BuildJob.hpp"

#include <span>
#include <utl/hashset.hpp>

#include "Core/Debug.hpp"
#include "Core/Network/Network.hpp"
#include "Core/Network/NodeImplementation.hpp"
#include "Core/Network/NodeDependencyMap.hpp"


#if 1
#define LOG_COORD(...) WM_Log(__VA_ARGS__)
#else
#define LOG_COORD(...)
#endif

namespace worldmachine {

	template <typename T>
	bool checkThrows(utl::invocable auto&& f) {
		try {
			f();
			return false;
		}
		catch (T const&) {
			return true;
		}
	}
	
	static void pruneIllFormedNodes(Network const* network, utl::vector<utl::UUID>* nodeIDs) {
		auto& ids = *nodeIDs;
	
		for (auto i = ids.begin(); i != ids.end();) {
			auto const id = *i;
			if (!network->allMandatoryUpstreamNodesConnected(id)) {
				WM_Log(warning, "Upstream dependecies for node \"{}\" not connected",
					   network->nodes().get<Node::Name>(network->indexFromID(id)));
				i = ids.erase(i);
			}
			else {
				++i;
			}
		}
	}
	
	static bool isInnerNode(Network const* network, utl::UUID id, std::span<utl::UUID const> ids) {
		std::size_t index = network->indexFromID(id);
		return checkThrows<int>([&]{
			network->traverseDownstreamNodes(index, [&](std::size_t downstreamIndex){
				auto const downstreamID = network->IDFromIndex(downstreamIndex);
				if (downstreamIndex != index && std::find(ids.begin(), ids.end(), downstreamID) != ids.end()) {
					throw int{};
				}
			});
		});
	}
	
	static void pruneInnerNodes(Network const* network, utl::vector<utl::UUID>* nodeIDs) {
		auto& ids = *nodeIDs;
	
		for (auto i = ids.begin(); i != ids.end();) {
			auto const id = *i;
			if (isInnerNode(network, id, *nodeIDs)) {
				i = ids.erase(i);
			}
			else {
				++i;
			}
		}
	}
	
	static void traverseUnique(Network const* network,
							   std::span<std::size_t const> nodeIndices,
							   utl::invocable<std::size_t> auto&& f)
	{
		utl::hashset<std::size_t> visited;
		visited.reserve(nodeIndices.size());
		for (auto leafIndex: nodeIndices) {
			network->traverseUpstreamNodes(leafIndex, [&](std::size_t nodeIndex){
				if (visited.insert(nodeIndex).second) {
					f(nodeIndex);
				}
			});
		}
		
	}
	
	BuildSystem::BuildSystem() {
		
	}
	
	utl::unique_ref<BuildSystem> BuildSystem::create() {
		return utl::unique_ref<BuildSystem>(new BuildSystem());
	}
	
	BuildSystem::~BuildSystem() {
		if (isBuilding()) {
			cancelCurrentBuild();
		}
		if (coordThread.joinable()) {
			coordThread.join();
		}
	}
	
	utl::vector<utl::listener> BuildSystem::makeListeners() {
		utl::vector<utl::listener> result;
		result.push_back(utl::make_listener([this](BuildRequest r) {
			this->build(r.buildType, r.network, std::move(r.nodes));
		}));
		result.push_back(utl::make_listener([this](BuildCancelRequest){
			cancelCurrentBuild();
		}));
		
		return result;
	}
	
	utl::small_vector<utl::UUID, 8> BuildSystem::gatherUnbuiltRoots(Network const* network,
																	std::span<utl::UUID const> leaves) const
	{
		utl::hashset<utl::UUID> unbuiltRoots;
		
		auto const indices = network->indicesFromIDs(leaves);
		for (auto const index: indices) {
			network->traverseUpstreamNodes(index, [&](std::size_t nodeIndex) {
				auto const nodeID = network->IDFromIndex(nodeIndex);
				if (allUpstreamNodesAreBuilt(network, nodeIndex) &&
					!buildingNodes.contains(nodeID) &&
					!builtNodes.contains(nodeID))
				{
					unbuiltRoots.insert(network->IDFromIndex(nodeIndex));
					return false;
				}
				return true;
			});
		}
		
		utl::small_vector<utl::UUID, 8> result(unbuiltRoots.begin(), unbuiltRoots.end());
		return result;
	}
	
	bool BuildSystem::allUpstreamNodesAreBuilt(Network const* network, std::size_t nodeIndex) const {
		return !checkThrows<int>([&]{
			network->traverseUpstreamNodes(nodeIndex, [&](std::size_t upstreamIndex) {
				if (upstreamIndex != nodeIndex && !builtNodes.contains(network->IDFromIndex(upstreamIndex)))
				{
					throw int{};
				}
			});
		});
	}
	
	utl::vector<utl::UUID> BuildSystem::performSanityChecks(Network const* network,
															utl::vector<utl::UUID> initial) const
	{
		pruneIllFormedNodes(network, &initial);
		pruneInnerNodes(network, &initial);
		return initial;
	}
	
	std::size_t BuildSystem::calculateTotalTargetBuildCount(Network const* network,
															std::span<utl::UUID const> leaves) const
	{
		auto const indices = network->indicesFromIDs(leaves);
		std::size_t result = 0;
		traverseUnique(network, indices, [&](std::size_t) {
			++result;
		});
		return result;
	}
	
	NodeDependencyMap BuildSystem::gatherDependencies(Network* network,
													  std::size_t nodeIndex,
													  BuildType buildType)
	{
		auto const edges = network->collectNodeEdges(nodeIndex);
		
		NodeDependencyMap dependencies;
		
		[&](auto const&... ec){
			return ([&](auto const& edgeCollection){
				for (auto& edge: edgeCollection) {
					if (edge.present) {
						WM_Assert(nodeIndex == edge.endNodeIndex, " ");
						auto const inputNodeIndex = edge.beginNodeIndex;
						WM_BoundsCheck(inputNodeIndex, 0, network->nodes().size());
						auto const testFlag = buildType == BuildType::highResolution ? NodeFlags::built : NodeFlags::previewBuilt;
						WM_Assert(!!(network->nodes().get<Node::Flags>(inputNodeIndex) & testFlag), "upstream node should have been build by now");
						auto desc = network->nodes().get<NodePinDescriptorArray>(nodeIndex).get(edge.endPinKind)[edge.endPinIndex];
						dependencies.inputs.insert(decltype(dependencies.inputs)::value_type{
							std::pair{ edge.endPinIndex, edge.endPinKind },
							typename NodeDependencyMap::InputDependency {
								network->nodes().get<Node::Implementation>(edge.beginNodeIndex).get(),
								edge.beginPinIndex
							}
						});
					}
					else /* edge not present */ {
						WM_Assert(!edge.mandatory, "Should be handled above");
						continue;
					}
				}
				return true;
			}(ec) && ...);
		}(edges.inputEdges, edges.maskInputEdges);
		
		return dependencies;
	}
	
	void BuildSystem::nodeBuildFinished(Network* network, utl::UUID nodeID, bool success) {
		network->locked([&]{
			auto const nodeIndex = network->indexFromID(nodeID);
			network->nodes().get<Node::BuildProgress>(nodeIndex) = 0;
			auto* const impl = network->nodes().get<Node::Implementation>(nodeIndex).get();
			impl->_isBuilding = false;
			if (currentBuildType() == BuildType::preview)
				impl->_previewBuilt = success;
			if (currentBuildType() == BuildType::highResolution)
				impl->_built = success;
			if (success) {
				if (currentBuildType() == BuildType::highResolution) {
					network->nodes().get<Node::Flags>(nodeIndex) |= NodeFlags::built;
				}
				else {
					network->nodes().get<Node::Flags>(nodeIndex) |= NodeFlags::previewBuilt;
				}
			}
			network->nodes().get<Node::Flags>(nodeIndex) &= ~NodeFlags::building;
			if (success) {
				WM_Log(info, "Finished building '{}'", network->nodes().get<Node::Name>(nodeIndex));
			}
			else {
				WM_Log(warning, "Cancelled build of '{}' [index = {}]", network->nodes().get<Node::Name>(nodeIndex), nodeIndex);
			}
		});
		
		
		
		std::unique_lock lock(coordMutex);
		[[maybe_unused]] auto const eraseResult = buildingNodes.erase(nodeID);
		WM_Assert(eraseResult == 1, "This node must have been in 'buildingNodes'");
		[[maybe_unused]] auto const insertResult = builtNodes.insert(nodeID).second;
		WM_Assert(insertResult, "This node mustn't have been in 'builtNodes'");
		++nodeBuildsCompleted;
		signal = Signal::start;
		coordCV.notify_one();
	}
	
	void BuildSystem::buildCoordinator(Network* network,
									   utl::vector<utl::UUID> nodes)
	{
		std::unique_lock lock(coordMutex);
		while (true) {
			switch (signal) {
				case Signal::sleep:
					LOG_COORD(info, "coordinator wakeup: case Signal::sleep:");
					coordSleep(lock);
					break;
					
				case Signal::start: {
					LOG_COORD(info, "coordinator wakeup: case Signal::start:");
					coordStart(network, nodes);
					break;
				}
					
				case Signal::finished:
					LOG_COORD(info, "coordinator wakeup: case Signal::finished:");
					mainCV.notify_one();
					return;
					
				case Signal::cancelBuild:
					LOG_COORD(info, "coordinator wakeup: case Signal::cancelBuild");
					coordCancel(network, lock);
					return;
					
				case Signal::none:
					WM_DebugBreak("Signal must never be none");
					break;
			}
		}
		LOG_COORD(warning, "Finished build");
	}
	
	void BuildSystem::coordSleep(std::unique_lock<std::mutex>& lock) {
		coordCV.wait(lock, [this]{ return signal != Signal::sleep; });
	}
	
	void BuildSystem::coordStart(Network* network, std::span<utl::UUID const> nodes) {
		if (totalTargetBuildCount == nodeBuildsCompleted) {
			// we are done!
			WM_Assert(buildingNodes.empty());
			cleanup(network);
			signal = Signal::finished;
			return;
		}
		
		auto const unbuildRoots = gatherUnbuiltRoots(network, nodes);
		if (unbuildRoots.empty()) {
			LOG_COORD(debug, "No unbuilt roots, going back to sleep");
			signal = Signal::sleep;
			return;
		}
		
		buildingNodes.insert(unbuildRoots.begin(), unbuildRoots.end());
		
		LOG_COORD(debug, "Unbuild roots are:");
		for ([[maybe_unused]] auto id: unbuildRoots) {
			LOG_COORD("    {} [index = {}, id = {}]", network->nodes().get<Node::Name>(network->indexFromID(id)),
				   network->indexFromID(id), id);
		}
		
		// gather all the current build jobs
		utl::hashmap<utl::UUID, BuildJob> currentBuildJobs;
		try {
			for (auto id: unbuildRoots) {
				std::size_t const nodeIndex = network->indexFromID(id);
				auto* impl = network->nodes().get<Node::Implementation>(nodeIndex).get();
				WM_Assert(impl->currentBuildType() == this->currentBuildType());
				WM_Assert(impl->currentBuildResolution() == this->currentBuildResolution());
				
				if (impl->type() == NodeType::image) {
					static_cast<ImageNodeImplementation*>(impl)->clearBuildDest();
				}
				[[maybe_unused]] bool const insertResult = currentBuildJobs.insert({
					id, impl->makeBuildJob(gatherDependencies(network, nodeIndex, currentBuildType()))
				}).second;
				WM_Assert(insertResult, "Node was build already");
			}
		}
		catch (BuildError const& e) {
			WM_Log(error, "Build Error: '{}'", e.what());
			signal = Signal::cancelBuild;
			return;
		}
		
		// dispatch all current build jobs
		for (auto& [nodeID, buildJob]: currentBuildJobs) {
			utl::dispatch_group g;
			std::size_t const nodeIndex = network->indexFromID(nodeID);
			network->locked([&]{
				network->nodes().get<Node::Flags>(nodeIndex) |= NodeFlags::building;
			});
			
			
			auto const oneProgress = buildJob.oneProgress();
			while (buildJob.hasJobs()) {
				auto oneJob = buildJob.consumeOne();
				auto jobWrapper = [=, oneJob = std::move(oneJob)] {
					oneJob();
					network->locked([&]{
						network->nodes().get<Node::BuildProgress>(nodeIndex) += oneProgress;
					});
					_info._progress += std::uint32_t(oneProgress * UINT_MAX / totalTargetBuildCount);
					network->_buildInfo._progress += std::uint32_t(oneProgress * UINT_MAX / totalTargetBuildCount);
					invalidateView();
				};
				g.add(std::move(jobWrapper));
			}
			g.on_completion([this, network, nodeID = nodeID,
							 nodeCompletionHandler = std::move(buildJob.completionHandler),
							 nodeCleanupHandler = buildJob.cleanupHandler]{
				if (nodeCompletionHandler)
					nodeCompletionHandler();
				if (nodeCleanupHandler)
					nodeCleanupHandler();
				nodeBuildFinished(network, nodeID, true);
			});
			g.on_failure([this, network, nodeID = nodeID, nodeCleanupHandler = buildJob.cleanupHandler]{
				if (nodeCleanupHandler)
					nodeCleanupHandler();
				nodeBuildFinished(network, nodeID, false);
			});
			network->nodes().get<Node::Implementation>(nodeIndex)->_isBuilding = true;
			dispatchQueue.async(std::move(g));
		}
		signal = Signal::sleep;
	}
	
	void BuildSystem::coordCancel(Network* network, std::unique_lock<std::mutex>& lock) {
		lock.unlock();
		dispatchQueue.cancel_current_tasks();
		lock.lock();
		LOG_COORD(debug, "locking network");
		network->locked([&]{
			for (auto id: buildingNodes) {
				network->nodes().get<Node::BuildProgress>(network->indexFromID(id)) = 0;
			}
		});
		LOG_COORD(debug, "cleanup");
		cleanup(network);
		LOG_COORD(debug, "done");
		mainCV.notify_one();
	}
	
	void BuildSystem::build(BuildType type,
							Network* network,
							utl::vector<utl::UUID> nodes)
	{
		WM_Expect(type != BuildType::none);
		if (isBuilding()) {
			WM_Log(error, "We are already building, ignoring this request");
			return;
		}
		
		WM_Assert(builtNodes.empty());
		WM_Assert(buildingNodes.empty());
		
		if (nodes.empty()) {
			nodes = network->IDsFromIndices(network->gatherLeaveNodes());
		}
		
		_info._type = type;
		_info._progress = 0;
		network->_buildInfo = _info;
		signal = Signal::start;
		builtNodes.clear();
		if (coordThread.joinable()) {
			coordThread.join();
		}
		stopwatch.reset();
		nodes = performSanityChecks(network, std::move(nodes));
		if (nodes.empty()) {
			WM_Log(warning, "'nodes' was empty. Not building anything");
			cleanup(network);
			return;
		}
		
		totalTargetBuildCount = calculateTotalTargetBuildCount(network, nodes);
		traverseUnique(network, network->indicesFromIDs(nodes),
					   [this, network](std::size_t nodeIndex){
			auto flags = network->nodes().get<Node::Flags>(nodeIndex);
			WM_Assert(!(flags & NodeFlags::building), "This node should not be building right now");
			auto const isBuiltFlag = _info.type() == BuildType::highResolution ? NodeFlags::built : NodeFlags::previewBuilt;
			if (test(flags & isBuiltFlag)) {
				builtNodes.insert(network->IDFromIndex(nodeIndex));
				++nodeBuildsCompleted;
				_info._progress += UINT_MAX / totalTargetBuildCount;
			}
			auto* const impl = network->nodes().get<Node::Implementation>(nodeIndex).get();
			
			impl->_currentBuildType = this->currentBuildType();
			impl->_previewBuildResolution = this->previewResolution;
			impl->_highresBuildResolution = this->resolution;
		});
		LOG_COORD(debug, "Sanity checks completed. Now Building {} Nodes. Leaf nodes are:", totalTargetBuildCount);
		for ([[maybe_unused]] auto id: nodes) {
			LOG_COORD("    {}", network->nodes().get<Node::Name>(network->indexFromID(id)));
		}
		LOG_COORD(debug, "Starting coordinator");
		coordThread = std::thread([=, nodes = std::move(nodes)]{
			buildCoordinator(network, std::move(nodes));
		});
	}
	
	void BuildSystem::cancelCurrentBuild() {
		std::unique_lock lock(coordMutex);
		signal = Signal::cancelBuild;
		coordCV.notify_one();
		
		mainCV.wait(lock, [&]{
			return !isBuilding();
		});
		
		dispatchQueue.wait_for_current_tasks();
	}
	
	
	void BuildSystem::cleanup(Network* network) {
		builtNodes.clear();
		buildingNodes.clear();
		_info = {};
		network->_buildInfo = {};
		totalTargetBuildCount = 0;
		nodeBuildsCompleted = 0;
		if (signal == Signal::finished || signal == Signal::start) {
			WM_Log(info, "Build finished in {}s",
				   double(stopwatch.elapsed_time()) / 1'000'000'000);
		}
		else if (signal == Signal::cancelBuild) {
			WM_Log(warning, "Build cancelled. {}s elapsed",
				   double(stopwatch.elapsed_time()) / 1'000'000'000);
		}
		else {
			WM_DebugBreak("Invalid signal");
		}
		
	}
	
	
}
