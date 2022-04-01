#include "EventMacOS.hpp"

#import <Carbon/Carbon.h>

namespace worldmachine {
	
	KeyCode translateKeyCode(int keyCode) {
		switch (keyCode) {
		   case kVK_ANSI_A:              return KeyCode::A;
		   case kVK_ANSI_S:              return KeyCode::S;
		   case kVK_ANSI_D:              return KeyCode::D;
		   case kVK_ANSI_F:              return KeyCode::F;
		   case kVK_ANSI_H:              return KeyCode::H;
		   case kVK_ANSI_G:              return KeyCode::G;
		   case kVK_ANSI_Z:              return KeyCode::Z;
		   case kVK_ANSI_X:              return KeyCode::X;
		   case kVK_ANSI_C:              return KeyCode::C;
		   case kVK_ANSI_V:              return KeyCode::V;
		   case kVK_ANSI_B:              return KeyCode::B;
		   case kVK_ANSI_Q:              return KeyCode::Q;
		   case kVK_ANSI_W:              return KeyCode::W;
		   case kVK_ANSI_E:              return KeyCode::E;
		   case kVK_ANSI_R:              return KeyCode::R;
		   case kVK_ANSI_Y:              return KeyCode::Y;
		   case kVK_ANSI_T:              return KeyCode::T;
		   case kVK_ANSI_1:              return KeyCode::_1;
		   case kVK_ANSI_2:              return KeyCode::_2;
		   case kVK_ANSI_3:              return KeyCode::_3;
		   case kVK_ANSI_4:              return KeyCode::_4;
		   case kVK_ANSI_6:              return KeyCode::_6;
		   case kVK_ANSI_5:              return KeyCode::_5;
		   case kVK_ANSI_Equal:          return KeyCode::equal;
		   case kVK_ANSI_9:              return KeyCode::_9;
		   case kVK_ANSI_7:              return KeyCode::_7;
		   case kVK_ANSI_Minus:          return KeyCode::minus;
		   case kVK_ANSI_8:              return KeyCode::_8;
		   case kVK_ANSI_0:              return KeyCode::_0;
		   case kVK_ANSI_RightBracket:   return KeyCode::rightBracket;
		   case kVK_ANSI_O:              return KeyCode::O;
		   case kVK_ANSI_U:              return KeyCode::U;
		   case kVK_ANSI_LeftBracket:    return KeyCode::leftBracket;
		   case kVK_ANSI_I:              return KeyCode::I;
		   case kVK_ANSI_P:              return KeyCode::P;
		   case kVK_ANSI_L:              return KeyCode::L;
		   case kVK_ANSI_J:              return KeyCode::J;
		   case kVK_ANSI_Quote:          return KeyCode::apostrophe;
		   case kVK_ANSI_K:              return KeyCode::K;
		   case kVK_ANSI_Semicolon:      return KeyCode::semicolon;
		   case kVK_ANSI_Backslash:      return KeyCode::backslash;
		   case kVK_ANSI_Comma:          return KeyCode::comma;
		   case kVK_ANSI_Slash:          return KeyCode::slash;
		   case kVK_ANSI_N:              return KeyCode::N;
		   case kVK_ANSI_M:              return KeyCode::M;
		   case kVK_ANSI_Period:         return KeyCode::period;
		   case kVK_ANSI_Grave:          return KeyCode::graveAccent;
		   case kVK_ANSI_KeypadDecimal:  return KeyCode::keypadDecimal;
		   case kVK_ANSI_KeypadMultiply: return KeyCode::keypadMultiply;
		   case kVK_ANSI_KeypadPlus:     return KeyCode::keypadAdd;
		   case kVK_ANSI_KeypadClear:    return KeyCode::numLock;
		   case kVK_ANSI_KeypadDivide:   return KeyCode::keypadDivide;
		   case kVK_ANSI_KeypadEnter:    return KeyCode::keypadEnter;
		   case kVK_ANSI_KeypadMinus:    return KeyCode::keypadSubtract;
		   case kVK_ANSI_KeypadEquals:   return KeyCode::keypadEqual;
		   case kVK_ANSI_Keypad0:        return KeyCode::keypad0;
		   case kVK_ANSI_Keypad1:        return KeyCode::keypad1;
		   case kVK_ANSI_Keypad2:        return KeyCode::keypad2;
		   case kVK_ANSI_Keypad3:        return KeyCode::keypad3;
		   case kVK_ANSI_Keypad4:        return KeyCode::keypad4;
		   case kVK_ANSI_Keypad5:        return KeyCode::keypad5;
		   case kVK_ANSI_Keypad6:        return KeyCode::keypad6;
		   case kVK_ANSI_Keypad7:        return KeyCode::keypad7;
		   case kVK_ANSI_Keypad8:        return KeyCode::keypad8;
		   case kVK_ANSI_Keypad9:        return KeyCode::keypad9;
		   case kVK_Return:              return KeyCode::enter;
		   case kVK_Tab:                 return KeyCode::tab;
		   case kVK_Space:               return KeyCode::space;
		   case kVK_Delete:              return KeyCode::backspace;
		   case kVK_Escape:              return KeyCode::escape;
		   case kVK_CapsLock:            return KeyCode::capsLock;
		   case kVK_Control:             return KeyCode::leftCtrl;
		   case kVK_Shift:               return KeyCode::leftShift;
		   case kVK_Option:              return KeyCode::leftAlt;
		   case kVK_Command:             return KeyCode::leftSuper;
		   case kVK_RightControl:        return KeyCode::rightCtrl;
		   case kVK_RightShift:          return KeyCode::rightShift;
		   case kVK_RightOption:         return KeyCode::rightAlt;
		   case kVK_RightCommand:        return KeyCode::rightSuper;
		   case kVK_F5:                  return KeyCode::F5;
		   case kVK_F6:                  return KeyCode::F6;
		   case kVK_F7:                  return KeyCode::F7;
		   case kVK_F3:                  return KeyCode::F3;
		   case kVK_F8:                  return KeyCode::F8;
		   case kVK_F9:                  return KeyCode::F9;
		   case kVK_F11:                 return KeyCode::F11;
		   case kVK_F13:                 return KeyCode::printScreen;
		   case kVK_F10:                 return KeyCode::F10;
		   case 0x6E:                    return KeyCode::menu;
		   case kVK_F12:                 return KeyCode::F12;
		   case kVK_Help:                return KeyCode::insert;
		   case kVK_Home:                return KeyCode::home;
		   case kVK_PageUp:              return KeyCode::pageUp;
		   case kVK_ForwardDelete:       return KeyCode::delete_;
		   case kVK_F4:                  return KeyCode::F4;
		   case kVK_End:                 return KeyCode::end;
		   case kVK_F2:                  return KeyCode::F2;
		   case kVK_PageDown:            return KeyCode::pageDown;
		   case kVK_F1:                  return KeyCode::F1;
		   case kVK_LeftArrow:           return KeyCode::leftArrow;
		   case kVK_RightArrow:          return KeyCode::rightArrow;
		   case kVK_DownArrow:           return KeyCode::downArrow;
		   case kVK_UpArrow:             return KeyCode::upArrow;
		   default:                      return KeyCode::none;
	   }
	}

	static bool isTrackpadScroll(NSEvent* event) {
		return event.phase != 0 || event.momentumPhase != 0;
	}

	Event toEvent(NSEvent* event) {
		return {
			.modifierFlags    = (EventModifierFlags)((unsigned)event.modifierFlags >> 16)
		};
	}

	KeyEvent toKeyEvent(NSEvent* event) {
		return {
			toEvent(event),
			.keyCode = translateKeyCode(event.keyCode)
		};
	}

	MouseEvent toMouseEvent(NSEvent* event) {
		return {
			toEvent(event),
			.locationInWindow = { event.locationInWindow.x, event.locationInWindow.y },
			.locationInView   = { event.locationInWindow.x, event.locationInWindow.y }
		};
	}
	
	MouseDownEvent toMouseDownEvent(NSEvent* event) {
		return {
			toMouseEvent(event),
			.clickCount = event.clickCount
		};
	}

	MouseUpEvent toMouseUpEvent(NSEvent* event) {
		return {
			toMouseEvent(event)
		};
	}

	MouseDragEvent toMouseDraggedEvent(NSEvent* event) {
		return {
			toMouseEvent(event),
			.offset = { event.deltaX, event.deltaY }
		};
	}

	ScrollEvent toScrollEvent(NSEvent* event) {
		return {
			toMouseEvent(event),
			.offset     = { event.scrollingDeltaX, event.scrollingDeltaY },
			.isTrackpad = isTrackpadScroll(event)
		};
	}

	MagnificationEvent toMagnificationEvent(NSEvent* event) {
		return {
			toMouseEvent(event),
			.offset = event.magnification
		};
	}

	MouseMoveEvent toMouseMoveEvent(NSEvent* event) {
		return {
			toMouseEvent(event),
			.offset = { event.deltaX, event.deltaY }
		};
	}

	
}
