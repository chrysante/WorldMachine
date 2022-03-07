#pragma once

#include <utl/hashmap.hpp>

#include "Node.hpp"
#include "NodeImplementation.hpp"

namespace worldmachine {
	
	class NodeDependencyMap {
		friend class BuildSystem;
	public:
		template <typename ValueType>
		ImageView<ValueType const> getInput(std::size_t index) {
			return getInputImpl<ValueType>(index, PinKind::input);
		}
		template <typename ValueType>
		ImageView<ValueType const> getMaskInput(std::size_t index) {
			return getInputImpl<ValueType>(index, PinKind::maskInput);
		}
		
	private:
		template <typename ValueType>
		ImageView<ValueType const> getInputImpl(std::size_t index, PinKind kind) {
			auto const itr = inputs.find({ index, kind });
			if (itr == inputs.end()) {
				return {};
			}
			
			auto const* const inputNode = dynamic_cast<ImageNodeImplementation const*>(itr->second.node);
			if (!inputNode) {
				return {};
			}
			return inputNode->getImage(itr->second.outputIndex, inputNode->_currentBuildType);
		}
		
	private:
		using InputDependencyKey = std::pair<std::size_t, PinKind>;
		
		struct InputDependency {
			NodeImplementation* node;
			std::size_t outputIndex;
		};
		
		utl::hashmap<
			InputDependencyKey,
			InputDependency,
			utl::hash<InputDependencyKey>
		> inputs;
	};
	
}
