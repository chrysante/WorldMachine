#include <Catch2/Catch2.hpp>

#include "Core/Network/Network.hpp"
#include "Core/Network/NetworkTraversal.hpp"

using namespace worldmachine;

namespace {
	struct MockNetworkBuilder {
		
		struct PinID {
			PinID(std::size_t nodeIndex, std::size_t pinIndex = 0):
				nodeIndex(nodeIndex),
				pinIndex(pinIndex)
			{}
			std::size_t nodeIndex;
			std::size_t pinIndex;
		};
		
		MockNetworkBuilder(): network(Network::create()) {
			desc = {
				.pinDescriptorArray = {
					.input = {
						PinDescriptor{ "", DataType::float1 },
						PinDescriptor{ "", DataType::float1 }
					},
					.output = {
						PinDescriptor{ "", DataType::float1 }
					}
				}
			};
		}
		
		utl::unique_ref<Network> get() { return std::move(network); }
		
		void addNodes(std::size_t size) {
			for (std::size_t i = 0; i < size; ++i) {
				addNode();
			}
		}
		
		void addNode(std::string_view name = {}) {
			desc.name = name;
			network->addNode(desc);
		}
		
		void addEdge(PinID from, PinID to) {
			network->addEdge(PinIndex{ from.nodeIndex, from.pinIndex, PinKind::output },
							 PinIndex{ to.nodeIndex, to.pinIndex, PinKind::input });
		}
		
	private:
		utl::unique_ref<Network> network;
		NodeDescriptor desc;
	};
}

TEST_CASE("Network Traversal simple") {
	MockNetworkBuilder builder;
	builder.addNodes(4);
	builder.addEdge(0, 1);
	builder.addEdge(0, 2);
	builder.addEdge(1, 3);
	builder.addEdge(2, {3, 1});
	
	/* Network:
		   1
		 /   \
	 0 -	   - 3
		 \   /
	       2
	 */
	
	auto network = builder.get();
	
	CHECK(!hasCycles(network.get(), 0));
	
	SECTION("Downstream Traversal") {
		std::array expectedIndexSequence = { 0, 1, 3, 2 };
		for (std::size_t index = 0;
			 std::size_t nodeIndex: NetworkTraversalView(network.get(), 0).unique())
		{
			REQUIRE(index < 4);
			CHECK(nodeIndex == expectedIndexSequence[index]);
			
			++index;
		}
	}
	
	SECTION("Upstream Traversal") {
		std::array expectedIndexSequence = { 3, 1, 0, 2 };
		for (std::size_t index = 0;
			 std::size_t nodeIndex: utl::reverse(NetworkTraversalView(network.get(), 3).unique()))
		{
			CHECK(nodeIndex == expectedIndexSequence[index]);
			++index;
		}
	}
}

TEST_CASE("Network Traversal more complex") {
	MockNetworkBuilder builder;

	builder.addNodes(7);

	builder.addEdge(0, 2);
	builder.addEdge(1, 3);

	builder.addEdge(2, {4, 0});
	builder.addEdge(2, {5, 0});
	builder.addEdge(3, {4, 1});
	builder.addEdge(3, {5, 1});

	builder.addEdge(4, {6, 0});
	builder.addEdge(5, {6, 1});

	/* Network:
	 0  -----  2  -----  4
				  \   /     \
					-         -- 6
	              /   \     /
	 1  -----  3  -----  5
	 */

	auto network = builder.get();
	
	CHECK(!hasCycles(network.get(), 0));
	CHECK(!hasCycles(network.get(), 1));

	SECTION("Downstream Traversal") {
		std::array expectedIndexSequence = { 0, 2, 4, 6, 5 };
		for (std::size_t index = 0;
			 std::size_t nodeIndex: NetworkTraversalView(network.get(), 0).unique())
		{
			CHECK(nodeIndex == expectedIndexSequence[index]);
			++index;
		}
	}

	SECTION("Upstream Traversal") {
		std::array expectedIndexSequence = { 6, 4, 2, 0, 3, 1, 5 };
		for (std::size_t index = 0;
			 std::size_t nodeIndex: utl::reverse(NetworkTraversalView(network.get(), 6).unique()))
		{
			CHECK(nodeIndex == expectedIndexSequence[index]);
			++index;
		}
	}
}


TEST_CASE("Network Cycle") {
	MockNetworkBuilder builder;
	builder.addNodes(4);
	builder.addEdge(0, 1);
	builder.addEdge(0, 2);
	builder.addEdge(1, 3);
	builder.addEdge(2, {3, 1});
	
	auto network = builder.get();
	
	// introduce cycle by adding edge from 3 to 0
	network->edges.push_back({
		.beginNodeIndex = 3,
		.endNodeIndex = 0,
		.beginPinIndex = 0,
		.endPinIndex = 0,
		.beginPinKind = PinKind::output,
		.endPinKind = PinKind::input
	});
	
	/* Network:
		   1
		 /   \
	 0 - <<--- - 3
		 \   /
		   2
	 */
	
	CHECK(hasCycles(network.get(), 0));
	
}
