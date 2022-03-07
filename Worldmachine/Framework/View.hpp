#pragma once

#include <string>
#include <mtl/mtl.hpp>

#include "Event.hpp"

namespace worldmachine {
	
	class Window;
	class InputSurfaceView;
	
	class View {
		friend class InputSurfaceView;
		friend class Window;
		
	public:
		View(std::string name):
			_name(std::move(name))
		{
			static unsigned id = 1;
			_childID = id++;
		}
		virtual ~View() = default;
		
		std::string_view name() const { return _name; }
		mtl::double2 position() const { return _position; }
		mtl::double2 size() const { return _size; }
		mtl::double2 scaleFactor() const { return _scaleFactor; }
		
		void hide() { _show = false; }
		void show() { _show = true; }
		bool isOpen() const { return _show; }
		bool isFocused() const { return _isFocused; }
		
	protected:
		void displayInactive(std::string_view reason);
		Window* getWindow() { return _window; }
		Window const* getWindow() const { return _window; }
		
	private:
		virtual void init() {}
		virtual void display() {}
		
		virtual bool keyDown(KeyEvent) { return false; }
		virtual bool keyUp(KeyEvent) { return false; }
		
	private:
		virtual void _doDisplay();
		
		
		/// To be used by base classes which are part of the framework
		///  in order to override _doDisplay()
		bool _beginDisplay();
		void _endDisplay();
		
		
	private:
		void _readViewInfo();
		bool _handleKeyEvent(KeyEvent, bool);
		
	private:
		std::string _name;
		unsigned _childID;
		mtl::double2 _position = 0, _size = 0, _scaleFactor = 1;
		void* _imguiWindow;
		Window* _window;
		bool _show = true;
		bool _isFocused = false;
	};
	
	void displayImage(void const* texture, mtl::double2 size);
	
	class InputSurfaceView: public virtual View {
		friend class Window;
	protected:
		InputSurfaceView(): View(std::string{}) {}
		void displayTexture(void const* texture) { worldmachine::displayImage(texture, this->size()); }
		
		mtl::double2 mouseLocationInView() const;
		
	private:
		virtual void mouseDown(MouseDownEvent) {}
		virtual void rightMouseDown(MouseDownEvent) {}
		virtual void otherMouseDown(MouseDownEvent) {}
		virtual void mouseUp(MouseUpEvent) {}
		virtual void rightMouseUp(MouseUpEvent) {}
		virtual void otherMouseUp(MouseUpEvent) {}
		virtual void mouseMoved(MouseMoveEvent) {}
		virtual void mouseDragged(MouseDragEvent) {}
		virtual void rightMouseMoved(MouseMoveEvent) {}
		virtual void rightMouseDragged(MouseDragEvent) {}
		virtual void otherMouseMoved(MouseMoveEvent) {}
		virtual void otherMouseDragged(MouseDragEvent) {}
		virtual void scrollWheel(ScrollEvent) {}
		virtual void magnify(MagnificationEvent) {}
		virtual void tick(TickEvent) {}
		
	private:
		void _doDisplay() override;
		void _handleEvent(EventUnion);
		
		mtl::double2 transformLocationToViewSpace(mtl::double2) const;
		
		template <typename EventT>
		void setLocationInView(EventT&, EventType);
		
	private:
		bool _hasMouseInputCapture = false;
	};
	
}
