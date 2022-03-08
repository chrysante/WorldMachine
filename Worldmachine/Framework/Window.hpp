#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mtl/mtl.hpp>
#include <utl/concepts.hpp>
#include <utl/functional.hpp>
#include <utl/messenger.hpp>

#include "Event.hpp"
#include "View.hpp"
#include "Appearance.hpp"

namespace worldmachine {

	class Window;
	namespace internal {
		std::string _getWindowTitle(Window const&);
		void _doDisplayWindow(Window&);
		void _windowHandleMouseEvent(Window&, EventUnion const&);
		bool _windowHandleKeyEvent(Window&, KeyEvent const&, bool);
		void _windowSetInvalidateFn(Window&, utl::function<void(std::size_t)>);
	}
	
	class Window {
		friend std::string internal::_getWindowTitle(Window const& w);
		friend void internal::_doDisplayWindow(Window&);
		friend void internal::_windowHandleMouseEvent(Window&, EventUnion const&);
		friend bool internal::_windowHandleKeyEvent(Window&, KeyEvent const&, bool);
		friend void internal::_windowSetInvalidateFn(Window&, utl::function<void(std::size_t)>);
	public:
		Window(std::string title);
		virtual ~Window();
		
		void invalidate(std::size_t dirty = 1) { _invalidate(dirty); }
		
		std::string_view title() const { return _title; }
		mtl::double2 size() const { return _size; }
		
		void sendMessage(utl::__message const& msg) {
			_messenger.send_message(msg);
		}
		
		[[nodiscard]] utl::listener_id registerListener(utl::listener_function auto&& l) {
			return _messenger.register_listener(UTL_FORWARD(l));
		}
		
		void removeListener(utl::listener_id& id) {
			_messenger.remove_listener(id);
		}
		
		void storeListenerID(utl::listener_id id) {
			_listenerIDs.insert(std::move(id));
		}
		
		View* findViewByName(std::string_view);
		View const* findViewByName(std::string_view) const;
		
		void forEachView(utl::invocable<View*> auto&& f) {
			for (auto& v: _views) {
				if constexpr (requires { (bool)f(nullptr); })
				{
					if (!f(v.get())) {
						break;
					}
				}
				else {
					f(v.get());
				}
			}
		}
		
		Appearance const& appearance() const { return _appearance; }
		
	private:
		/// For Base Classes to override
		virtual void display() {}
		virtual bool keyDown(KeyEvent) { return false; }
		virtual bool keyUp(KeyEvent) { return false; }
		
	protected:
		/// For use by Base Classes
		void addView(View*);
		utl::messenger& getMessenger() { return _messenger; }
		
	private:
		/// Actually private
		void _doDisplay();
		void _handleMouseEvent(EventUnion);
		bool _handleKeyEvent(KeyEvent, bool isDown);
		mtl::double2 _transformLocationAppKitToImGui(mtl::double2) const;
		mtl::double2 _transformDirectionAppKitToImGui(mtl::double2) const;
		
		void _setStyle();
		
	private:
		std::string _title;
		std::vector<std::unique_ptr<View>> _views;
		mtl::double2 _size = 0;
		Appearance _appearance = {};
		utl::function<void(std::size_t)> _invalidate;
		utl::messenger _messenger;
		utl::listener_id_bag _listenerIDs;
	};
	
	inline std::string internal::_getWindowTitle(Window const& w) {
		return w._title;
	}
	inline void internal::_doDisplayWindow(Window& w) {
		w._doDisplay();
	}
	inline void internal::_windowHandleMouseEvent(Window& w, EventUnion const& e) {
		w._handleMouseEvent(e);
	}
	inline bool internal::_windowHandleKeyEvent(Window& w, KeyEvent const& e, bool isDown) {
		return w._handleKeyEvent(e, isDown);
	}
	inline void internal::_windowSetInvalidateFn(Window& w, utl::function<void(std::size_t)> f) {
		w._invalidate = std::move(f);
	}
}
