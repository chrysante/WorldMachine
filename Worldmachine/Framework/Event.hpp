#pragma once

#include <mtl/mtl.hpp>
#include <utl/common.hpp>
#include <array>
#include <iosfwd>


namespace worldmachine {
	
	enum struct KeyCode {
		none = 0,
		A, S, D, F, H, G, Z, X, C, V, B, Q, W, E, R, Y, T, O, U, I, P, L, J, K, N, M,
		_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
		equal, minus,
		rightBracket,
		leftBracket,
		apostrophe,
		semicolon,
		backslash,
		comma,
		slash,
		period,
		graveAccent,
		keypadDecimal,
		keypadMultiply,
		keypadAdd,
		numLock,
		keypadDivide,
		keypadEnter,
		keypadSubtract,
		keypadEqual,
		keypad0,
		keypad1,
		keypad2,
		keypad3,
		keypad4,
		keypad5,
		keypad6,
		keypad7,
		keypad8,
		keypad9,
		enter,
		tab,
		space,
		backspace,
		escape,
		capsLock,
		leftCtrl,
		leftShift,
		leftAlt,
		leftSuper,
		rightCtrl,
		rightShift,
		rightAlt,
		rightSuper,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		printScreen,
		menu,
		insert,
		home,
		pageUp,
		delete_,
		end,
		pageDown,
		leftArrow,
		rightArrow,
		downArrow,
		upArrow
	};
	
	enum struct EventModifierFlags: unsigned {
		none       = 0,
		capsLock   = 1 << 0, // Set if Caps Lock key is pressed.
		shift      = 1 << 1, // Set if Shift key is pressed.
		control    = 1 << 2, // Set if Control key is pressed.
		option     = 1 << 3, // Set if Option or Alternate key is pressed.
		super      = 1 << 4, // Set if Command key is pressed.
		numericPad = 1 << 5, // Set if any key in the numeric keypad is pressed.
		help       = 1 << 6, // Set if the Help key is pressed.
		function   = 1 << 7  // Set if any function key is pressed.
	};
	
	UTL_ENUM_OPERATORS(EventModifierFlags);
	
	struct Event {
		EventModifierFlags modifierFlags;
	};
	
	struct MouseEvent: Event {
		mtl::double2 locationInView;
		mtl::double2 locationInWindow;
	};
	
	struct MouseDownEvent: MouseEvent {
		long clickCount;
	};
	
	struct MouseUpEvent: MouseEvent {
		
	};
	
	struct MouseDragEvent: MouseEvent {
		mtl::double2 offset;
	};
	
	struct ScrollEvent: MouseEvent {
		mtl::double2 offset;
		bool isTrackpad;
	};
	
	struct MagnificationEvent: MouseEvent {
		double offset;
	};
	
	struct MouseMoveEvent: MouseEvent {
		mtl::double2 offset;
	};
	
	struct TickEvent {
		double deltaTime;
	};
	
	struct KeyEvent: Event {
		KeyCode keyCode;
	};
	
	enum struct EventType {
		none = 0,
		mouseDown,
		rightMouseDown,
		otherMouseDown,
		mouseUp,
		rightMouseUp,
		otherMouseUp,
		mouseMoved,			//
		mouseDragged,		//
		rightMouseMoved,	//
		rightMouseDragged,	//
		otherMouseMoved,	//
		otherMouseDragged,	//
		scrollWheel,		//
		magnify,
		keyDown, keyUp
	};
	
	inline bool isMouseEvent(EventType type) {
		return (int)type != 0 && (int)type <= (int)EventType::magnify;
	}
	
	inline std::ostream& operator<<(std::ostream& str, EventType e) {
		return str << std::array{
			"mouseDown",
			"rightMouseDown",
			"otherMouseDown",
			"mouseUp",
			"rightMouseUp",
			"otherMouseUp",
			"mouseMoved",
			"mouseDragged",
			"rightMouseMoved",
			"rightMouseDragged",
			"otherMouseMoved",
			"otherMouseDragged",
			"scrollWheel",
			"magnify",
			"tick"
		}[(unsigned)e];
 	}
	
	struct EventUnion {
		EventType type;
		union {
			MouseEvent         mouseEvent;
			MouseDownEvent     mouseDownEvent;
			MouseUpEvent       mouseUpEvent;
			MouseMoveEvent     mouseMoveEvent;
			MouseDragEvent     mouseDragEvent;
			ScrollEvent        scrollEvent;
			MagnificationEvent magnificationEvent;
		};
	};
	
	
	
}

