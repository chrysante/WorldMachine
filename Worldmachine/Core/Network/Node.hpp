#pragma once

#include "Framework/Typography/TextRenderData.hpp"

#include <utl/structure_of_arrays.hpp>
#include <utl/memory.hpp>
#include <mtl/mtl.hpp>
#include <utl/UUID.hpp>
#include <string>
#include <array>
#include <optional>

#include "Pin.hpp"
#include "SharedNetworkTypes.hpp"
#include "ImplementationID.hpp"

namespace worldmachine {
	
	NodeParameters defaultNodeParameters();
	
	class NodeImplementation;
	
	struct NodeDescriptor {
		NodeCategory category;
		std::string name;
		NodePinDescriptorArray pinDescriptorArray;
		// Not to be set by Plugins
		mtl::float3 position;
		std::optional<ImplementationID> implementationID;
	};
	
	UTL_SOA_TYPE(Node,
				 (std::string,                  name),
				 (NodeCategory,                 category),
				 (mtl::float3,                  position),
				 (mtl::float2,                  size),
				 (float,                        buildProgress),
				 (PinCount<>,                   pinCount),
				 (NodeFlags,                    flags),
				 (utl::UUID,                    id),
				 (utl::ref<NodeImplementation>, implementation),
				 (NodePinDescriptorArray,       pinDescriptorArray)
				 );
	
	mtl::float2 nodeSize(NodeParameters, PinCount<float>);

	PinCount<> calculatePinCount(NodeDescriptor const&);
	
}
