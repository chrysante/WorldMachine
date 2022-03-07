#pragma once

#include "Core/Debug.hpp"
#include "BuildSystemFwd.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <span>
#include <utl/functional.hpp>
#include <utl/vector.hpp>
#include <utl/hashset.hpp>
#include <utl/hashmap.hpp>
#include <utl/UUID.hpp>
#include <utl/dispatch_queue.hpp>
#include <mtl/mtl.hpp>
#include <utl/stopwatch.hpp>

namespace worldmachine {
	
	class Network;
	class NodeDependencyMap;
	
	class BuildSystem {
		enum struct Signal {
			none = 0, sleep, start, finished, cancelBuild
		};
	public:
		~BuildSystem();
		
		void build(Network* network);
		void build(Network* network,
				   utl::vector<utl::UUID> nodes);
		
		void buildPreview(Network* network);
		void buildPreview(Network* network,
						  utl::vector<utl::UUID> nodes);
		
		bool isBuilding() const { return _isBuilding.load(); }
		BuildType currentBuildType() const { return _buildType; }
		void cancelCurrentBuild();
		
		void setViewInvalidator(utl::function<void()> f) {
			_invalidateView = f;
		}
		
		auto locked(utl::invocable auto&& f) {
			std::unique_lock lock(coordMutex);
			return f();
		}
		
		mtl::usize2 getResolution() { return resolution; }
		void setResolution(mtl::usize2 resolution) { WM_Assert(!isBuilding()); this->resolution = resolution; }
		mtl::usize2 getPreviewResolution() { return previewResolution; }
		void setPreviewResolution(mtl::usize2 resolution) { WM_Assert(!isBuilding()); this->previewResolution = resolution; }
		
		std::size_t getNumberOfThreads() const {
			return dispatchQueue.num_threads();
		}
		void setNumberOfThreads(std::size_t n) {
			dispatchQueue.set_num_threads(n);
		}
		
		double progress() const { return (double)_progress.load() / UINT_MAX; }
		
	private:
		void buildImpl(Network* network, utl::vector<utl::UUID> nodes, BuildType);
		void buildCoordinator(Network* network,
							  utl::vector<utl::UUID> nodes);
		void coordSleep(std::unique_lock<std::mutex>&);
		void coordStart(Network*, std::span<utl::UUID const> nodes);
		void coordCancel(Network* network, std::unique_lock<std::mutex>&);
		
		utl::vector<utl::UUID> performSanityChecks(Network const* network,
												   utl::vector<utl::UUID>) const;
		
		utl::small_vector<utl::UUID, 8> gatherUnbuiltRoots(Network const* network,
														   std::span<utl::UUID const> leaves) const;
		
		bool allUpstreamNodesAreBuilt(Network const* network, std::size_t nodeIndex) const;
		
		std::size_t calculateTotalTargetBuildCount(Network const*,
												   std::span<utl::UUID const>) const;
		
		NodeDependencyMap gatherDependencies(Network*, std::size_t nodeIndex, BuildType);
		
		void nodeBuildFinished(Network* network, utl::UUID nodeID, bool success);
		
		void cleanup();
		
		void invalidateView() {
			if (_invalidateView)
				_invalidateView();
		}
		
		mtl::usize2 currentBuildResolution() const {
			return _buildType == BuildType::highResolution ?
				resolution : previewResolution;
		}
		
	private:
		std::thread coordThread;
		std::mutex coordMutex;
		std::condition_variable coordCV;
		std::condition_variable mainCV;
		std::atomic_bool _isBuilding = false;
		
		utl::concurrent_dispatch_queue dispatchQueue{2};
		
		utl::hashset<utl::UUID> builtNodes;
		utl::hashset<utl::UUID> buildingNodes;
		
		utl::function<void()> _invalidateView;
		std::size_t totalTargetBuildCount = 0;
		std::size_t nodeBuildsCompleted = 0;
		Signal signal = Signal::start;
		BuildType _buildType;
		mtl::usize2 resolution = WM_DEBUGLEVEL == 2 ? 256 : 1024;
		mtl::usize2 previewResolution = WM_DEBUGLEVEL == 2 ? 64 : 256;
		utl::precise_stopwatch stopwatch;
		std::atomic<unsigned> _progress = 0;
	};
	
}
