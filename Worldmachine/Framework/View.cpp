#define IMGUI_DEFINE_MATH_OPERATORS

#include "View.hpp"

#include <utl/scope_guard.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Core/Debug.hpp"

using namespace mtl::short_types;

namespace worldmachine {
	
	/// MARK: - View
	void View::displayInactive(std::string_view msg) {
		double2 const textSize = (float2)ImGui::CalcTextSize(msg.data());
		ImGui::SetCursorPos((this->size() - textSize) / 2);
		ImGui::Text("%s", msg.data());
	}
	
	void View::_doDisplay() {
		if (_beginDisplay()) {
			
			_endDisplay();
		}
	}
	
	bool View::_beginDisplay() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, _padding);
		utl::scope_guard guardPadding = []{
			ImGui::PopStyleVar();
		};
		
		if (_backgroundColor) {
			ImGui::PushStyleColor(ImGuiCol_WindowBg, *_backgroundColor);
		}
		utl::scope_guard guardBackgroundColor = [this]{
			if (_backgroundColor) {
				ImGui::PopStyleColor();
			}
		};
		
		
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem(utl::format("Show {}", name()).data(),
								nullptr, _show))
			{
				_show ^= true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
		_isFocused = false;
		if (_show) {
			ImGui::Begin(_name.c_str(), &_show);
			_isFocused |= ImGui::IsWindowFocused();
			ImGui::BeginChild(_childID);
			_isFocused |= ImGui::IsWindowFocused();
			this->_readViewInfo();
			return true;
		}
		return false;
	}
	
	void View::_endDisplay() {
		this->display();

		ImGui::EndChild();
		ImGui::End();
	}
	
	void View::_readViewInfo() {
		ImGuiContext& g = *GImGui;
		this->_imguiWindow = g.CurrentWindow;
		_size        = ImGui::GetWindowSize();
		_position    = ImGui::GetWindowPos();
		_scaleFactor = ImGui::GetIO().DisplayFramebufferScale;
	}
	
	bool View::_handleKeyEvent(KeyEvent event, bool isDown) {
		if (isDown) {
			return this->keyDown(event);
		}
		else {
			return this->keyUp(event);
		}
	}
	
	
	/// MARK: - InputSurfaceView
	static bool detectViewportInput();
	
	InputSurfaceView::InputSurfaceView(): View(std::string{}) {
		setPadding(0);
#if WM_DEBUGLEVEL
//		setBackgroundColor({ 1, 0, 1, 1 });
#endif
	}
	
	void InputSurfaceView::_doDisplay() {
		if (View::_beginDisplay()) {
			_hasMouseInputCapture = detectViewportInput();
			_endDisplay();
		}
	}
	
	mtl::double2 InputSurfaceView::mouseLocationInView() const {
		return transformLocationToViewSpace(ImGui::GetIO().MousePos);
	}
	
	template<typename T>
	static constexpr bool hasOffset = requires (T&& t) { { t.offset } -> mtl::same_as<mtl::double2>; };
	
	template<typename T>
	static constexpr bool hasLocation = requires (T&& t) {
		{ t.locationInView } -> mtl::same_as<mtl::double2>;
		{ t.locationInWindow } -> mtl::same_as<mtl::double2>;
	};
	
	template <typename E>
	void maybeTransformOffset(E& e) {
		if constexpr (hasOffset<E>) {
			e.offset.y *= -1.0;
		}
	}
	
	void InputSurfaceView::_handleEvent(EventUnion event) {
#define WM_HANDLE_EVENT(FN, EVENT_TYPE)                \
case EventType::FN:                                    \
maybeTransformOffset(event.EVENT_TYPE);                \
setLocationInView(event.EVENT_TYPE,                    \
				  event.type);                         \
this->FN(event.EVENT_TYPE);                            \
return
		
		using V = InputSurfaceView;
		switch (event.type) {
			WM_HANDLE_EVENT(mouseDown,      mouseDownEvent);
			WM_HANDLE_EVENT(rightMouseDown, mouseDownEvent);
			WM_HANDLE_EVENT(otherMouseDown, mouseDownEvent);
			
			WM_HANDLE_EVENT(mouseUp,      mouseUpEvent);
			WM_HANDLE_EVENT(rightMouseUp, mouseUpEvent);
			WM_HANDLE_EVENT(otherMouseUp, mouseUpEvent);
				
			WM_HANDLE_EVENT(mouseMoved,      mouseMoveEvent);
			WM_HANDLE_EVENT(rightMouseMoved, mouseMoveEvent);
			WM_HANDLE_EVENT(otherMouseMoved, mouseMoveEvent);
			
			WM_HANDLE_EVENT(mouseDragged,      mouseDragEvent);
			WM_HANDLE_EVENT(rightMouseDragged, mouseDragEvent);
			WM_HANDLE_EVENT(otherMouseDragged, mouseDragEvent);
				
			WM_HANDLE_EVENT(scrollWheel, scrollEvent);
			WM_HANDLE_EVENT(magnify,     magnificationEvent);
				
			default:
				WM_DebugBreak("We don't want these events");
		}
#undef WM_HANDLE_EVENT
	}
	
	mtl::double2 InputSurfaceView::transformLocationToViewSpace(mtl::double2 p) const {
		p = p - this->position();
		p.y = this->size().y - p.y;
		return p;
	}
	
	template <typename E>
	void InputSurfaceView::setLocationInView(E& e, EventType type) {
		if constexpr (hasLocation<E>) {
			e.locationInView = transformLocationToViewSpace(e.locationInWindow);
		}
	}
	
	static bool detectViewportInput() {
		using namespace ImGui;
		
		// save to restore later
		ImVec2 const cursorPos = GetCursorPos();
		
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		const ImGuiID id = window->GetID("__Invisible_Button__");
		
		ImVec2 size = CalcItemSize(window->Size, 0.0f, 0.0f);
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
		ItemSize(size);
		if (!ItemAdd(bb, id))
			return {};

		auto const buttonFlags = ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle;
		
		bool pressed, held, hovered;
		pressed = ButtonBehavior(bb, id, &hovered, &held, buttonFlags);

		SetCursorPos(cursorPos);
		
		return pressed || held || hovered;
	}
	
}
