#pragma once

#include "Core/Plugin.hpp"

namespace worldmachine {
	
	class BlurNode: public ImageNodeImplementationT<BlurNode, "Blur"> {
	public:
		static NodeDescriptor staticDescriptor();
		
		bool displayControls() override;
		BuildJob makeBuildJob(NodeDependencyMap) override;
		
	private:
		
	};
	
}
