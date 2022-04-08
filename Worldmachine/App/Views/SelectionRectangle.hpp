#pragma once

#include <mtl/mtl.hpp>

using namespace mtl::short_types;

namespace worldmachine {
	
	class SelectionRectangle {
	public:
		SelectionRectangle(): _rect{}{}
		SelectionRectangle(mtl::float2 position): _rect(position, 0) {}
		
		void addOffset(mtl::float2 offset) {
			float2 origin = _rect.bottom_left();
			float2 size = _rect.size();
			for (int i = 0; i < 2; ++i) {
				if (offset[i] >= 0) {
					size[i] += offset[i];
				}
				else {
					origin[i] += offset[i];
				}
			}
			_rect = mtl::rectangle<float>(origin, size);
		}
		
		auto get() const { return _rect; }
		
	private:
		mtl::rectangle<float> _rect;
	};
	
}
