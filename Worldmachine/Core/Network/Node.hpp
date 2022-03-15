#pragma once

#include "Framework/Typography/TextRenderData.hpp"

#include <utl/structure_of_arrays.hpp>
#include <utl/named_type.hpp>
#include <utl/memory.hpp>
#include <mtl/mtl.hpp>
#include <utl/UUID.hpp>
#include <string>
#include <array>

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
		// not to be set by implementations
		mtl::float3 position;
		ImplementationID implementationID;
	};
	
	
	
	struct Node {
		using Name                     = utl::named_type<std::string,	struct NameTag>;
//		using NameRenderData           = utl::named_type<utl::array<LetterData, nodeNameMaxRenderSize>,
//																		struct NameRenderDataTag>;
		using Category                 = NodeCategory;
		using Position                 = utl::named_type<mtl::float3,	struct PositionTag>;
		using Size                     = utl::named_type<mtl::float2,	struct SizeTag>;
		
		using BuildProgress            = utl::named_type<float, struct BuildProgressTag>;
		using PinCount                 = worldmachine::PinCount<>;
		using Flags                    = NodeFlags;
		using ID                       = utl::named_type<utl::UUID,		struct IDTag>;;
		using Implementation           = utl::ref<NodeImplementation>;
		using PinDescriptorArray       = NodePinDescriptorArray;
	};
	
	
	using NodeSOAType = utl::soa::tuple<
		Node::Name,
//		Node::NameRenderData,
		Node::Category,
		Node::Position,
		Node::Size,
		Node::BuildProgress,
		Node::PinCount,
		Node::Flags,
		Node::ID,
		Node::Implementation,
		Node::PinDescriptorArray
	>;
	
	inline mtl::float2 nodeSize(NodeParameters nodeParams, PinCount<float> pinCount) {
		float const x = [&]{
			auto const iCount = std::max(pinCount.parameterInput, 3.0f);
			return iCount * nodeParams.parameterPinSpacing + 2.0f * nodeParams.cornerRadius;
		}();
		float const y = [&]{
			auto const iCount = std::max({ pinCount.input, pinCount.output, 1.0f });
			return iCount * nodeParams.pinSpacing + 2.0f * nodeParams.cornerRadius;
		}();
		return { x, y };
	}

	PinCount<> calculatePinCount(NodeDescriptor const&);
	
}
