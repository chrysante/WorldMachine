#pragma once

#include <exception>

#include <atomic>
#include <utl/common.hpp>
#include <utl/messenger.hpp>
#include <utl/vector.hpp>
#include <utl/UUID.hpp>

namespace worldmachine {
	
	class BuildSytem;
	class Network;
	
	enum struct BuildType {
		none = 0,
		highResolution = 1 << 0,
		preview = 1 << 1,
		all = (1 << 0) | (1 << 1)
	};
	UTL_ENUM_OPERATORS(BuildType);
	
	
	class BuildInfo {
	public:
		BuildInfo() = default;
		BuildInfo(BuildInfo const& rhs): _type(rhs._type.load()), _progress(rhs._progress.load()) {}
		BuildInfo& operator=(BuildInfo const& rhs) {
			_type = rhs._type.load();
			_progress = rhs._progress.load();
			return *this;
		}
		
		
		BuildType type() const { return _type; }
		bool isBuilding() const { return _type != BuildType::none; }
		double progress() const {
			return (double)_progress / std::numeric_limits<std::uint32_t>::max();
		}
		
		
		
	private:
		friend class BuildSystem; // build system is allowed to set these fields
		std::atomic<BuildType> _type{};
		std::atomic<std::uint32_t> _progress{};
	};
	
	class BuildError: public std::runtime_error {
	public:
		BuildError(std::string message): std::runtime_error(message) {}
	};
	
	class BuildJob;
	
	struct BuildRequest: utl::message<BuildRequest> {
		BuildRequest(BuildType buildType, Network* network, utl::vector<utl::UUID> nodes = {}):
			buildType(buildType), network(network), nodes(std::move(nodes))
		{}
		
		BuildType buildType;
		Network* network;
		utl::vector<utl::UUID> nodes;
	};
	
	struct BuildCancelRequest: utl::message<BuildCancelRequest> {
		
	};
	
}
