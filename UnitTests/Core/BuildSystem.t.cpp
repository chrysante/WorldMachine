#include <Catch2/Catch2.hpp>

#include "Core/BuildSystem.hpp"
#include "Core/Network/Network.hpp"

using namespace worldmachine;

TEST_CASE("BuildSystem") {
	auto buildSystem = BuildSystem::create();
	
	utl::messenger m;
	auto listeners = buildSystem->makeListeners();
	[[maybe_unused]] auto ids = m.register_listeners(listeners.begin(), listeners.end());
	
	
	auto network = Network::create();
	
	m.send_message(BuildRequest(BuildType::preview, network.get()));
	
}
