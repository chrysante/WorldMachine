#pragma once

#include <optional>
#include <array>
#include <utl/vector.hpp>

#include "Core/Network/Node.hpp"

namespace worldmachine {
	
	class CreateNodesMenu {
	public:
		bool display();
		
		NodeDescriptor selectedElement() const { return _selectedElement.value(); }
		
	private:
		std::array<utl::vector<NodeDescriptor>, (int)NodeCategory::count> nodeIDs;
		std::optional<NodeDescriptor> _selectedElement;
		bool _shown = false;
	};
	
}
