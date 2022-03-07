#pragma once

#include <mtl/mtl.hpp>

namespace worldmachine {
	
	class SelectionRectangle {
	public:
		SelectionRectangle(): _rect{}{}
		SelectionRectangle(mtl::float2 position): _rect{ .origin = position, .extend = 0 } {}
		
		void addOffset(mtl::float2 offset) {
			_rect.extend += offset;
		}
		
		auto get() const { return normalize(_rect); }
		
	private:
		mtl::rectangle<float> _rect;
	};
	
}
