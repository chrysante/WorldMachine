#include "Core/Network/Network.hpp"
#include "Core/Network/NetworkTraversal.hpp"

#include <iostream>
#include <stack>
#include <utl/vector.hpp>



int main() {
	using namespace worldmachine;
	auto network = Network::create();
	
	NodeDescriptor desc = {
		.pinDescriptorArray = {
			.input = {
				PinDescriptor{ "Default1", DataType::float1 },
				PinDescriptor{ "Default2", DataType::float1 }
			},
			.output = { PinDescriptor{ "Default", DataType::float1 } },
			.parameterInput = { PinDescriptor{ "Scale", DataType{} }, PinDescriptor{ "Offset", DataType{} } },
			.maskInput = {}
		}
	};
	desc.name = "Test 0";
	network->addNode(desc);
	desc.name = "Test 1";
	network->addNode(desc);
	desc.name = "Test 2";
	network->addNode(desc);
	desc.name = "Test 3";
	network->addNode(desc);
	
	network->addEdge(PinIndex{ 0, 0, PinKind::output },
					 PinIndex{ 1, 0, PinKind::input });
	network->addEdge(PinIndex{ 0, 0, PinKind::output },
					 PinIndex{ 2, 0, PinKind::input });
	
	network->addEdge(PinIndex{ 1, 0, PinKind::output },
					 PinIndex{ 3, 0, PinKind::input });
	network->addEdge(PinIndex{ 2, 0, PinKind::output },
					 PinIndex{ 3, 1, PinKind::input });
	
	
	
//	std::cout << "network->traverseDownstreamNodes ----------------------\n";
//	network->traverseDownstreamNodes(0, [&](std::size_t index) {
//		std::cout << network->nodes[index].name << std::endl;
//	});
//	std::cout << std::endl;
//	std::cout << "network->traverseDownstreamNodesReverse ---------------\n";
//	network->traverseDownstreamNodesReverse(0, [&](std::size_t index) {
//		std::cout << network->nodes[index].name << std::endl;
//	});
//	std::cout << std::endl;
	
	std::cout << "network->traverseDownstreamNodes --------------------\n";
	network->traverseDownstreamNodes(0, [&](std::size_t index) {
		std::cout << network->nodes[index].name << std::endl;
	});
	std::cout << std::endl;
	std::cout << "NetworkTraversalView --------------------------------\n";
	std::size_t count = 0;
	for (auto index: NetworkTraversalView(network.get(), 0).unique()) {
		std::cout << network->nodes[index].name << " [index = " << index << "]" << std::endl;
		if (count++ > 100)
			break;
	}
	std::cout << std::endl;
}
