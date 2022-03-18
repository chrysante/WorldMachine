#pragma once

#include "Framework/Event.hpp"

#import <Cocoa/Cocoa.h>

namespace worldmachine {
	
	KeyCode translateKeyCode(int keyCode);

	Event toEvent(NSEvent*);

	KeyEvent toKeyEvent(NSEvent*);

	MouseEvent toMouseEvent(NSEvent*);
	
	MouseDownEvent toMouseDownEvent(NSEvent*);

	MouseUpEvent toMouseUpEvent(NSEvent*);

	MouseDragEvent toMouseDraggedEvent(NSEvent*);

	ScrollEvent toScrollEvent(NSEvent*);

	MagnificationEvent toMagnificationEvent(NSEvent*);

	MouseMoveEvent toMouseMoveEvent(NSEvent*);
	
}
