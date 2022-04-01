#define IMGUI_DEFINE_MATH_OPERATORS

#include "View.hpp"

#include <utl/scope_guard.hpp>
#include <utl/typeinfo.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "ImGuiExt.hpp"

#include "Core/Debug.hpp"

using namespace mtl::short_types;

namespace worldmachine {
	
	/// MARK: - View
	View::View(std::string name):
		_name(std::move(name))
	{
		static unsigned id = 1;
		_childID = id += 10;
	}
	
	void View::displayInactive(std::string_view msg) {
		double2 const textSize = (float2)ImGui::CalcTextSize(msg.data());
		ImGui::SetCursorPos((this->size() - textSize) / 2);
		ImGui::Text("%s", msg.data());
	}
	
	void View::_doDisplay() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, _isInputSurfaceView ? double2{0, 0} : _padding);
		utl::armed_scope_guard guardPadding = []{
			ImGui::PopStyleVar();
		};
		
		_mainMenuBar();
		
		_isFocused = false;
		if (_show) {
			ImGui::Begin(_name.c_str(), &_show);
			_isFocused |= ImGui::IsWindowFocused();
			ImGui::BeginChild(_childID);
			_isFocused |= ImGui::IsWindowFocused();
			
			_readViewInfo();
			
			
			
			guardPadding.execute();
			_reallyDoDisplay();

			
			ImGui::EndChild();
			
			ImGui::End();
		}
	}
	
	void View::_reallyDoDisplay() {
		auto* w = (ImGuiWindow*)_imguiWindow;
		
		w->Pos += ImVec2{5, 5};
		w->Size -= ImVec2{10, 10};
		display();
		if (hasContextMenu() && ImGui::BeginPopupContextWindow("Context Menu")) {
			contextMenu();
			ImGui::EndPopup();
		}
	}
	
	void View::_mainMenuBar() {
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
	static bool detectViewportInput(ImGuiButtonFlags);
	
	InputSurfaceView::InputSurfaceView(): View(std::string{}) {
		_isInputSurfaceView = true;
//		setPadding(0);
#if WM_DEBUGLEVEL
//		setBackgroundColor({ 1, 0, 1, 1 });
#endif
	}
	
	void InputSurfaceView::_reallyDoDisplay() {
		
		ImGui::SetCursorPos({});
		display();
		
		auto* w = ((ImGuiWindow*)_imguiWindow);
		w->Pos += _padding;
		w->Size -= 2 * _padding;
		ImGui::SetCursorPos({});
		displayControls();
		w->Pos -= _padding;
		w->Size += 2 * _padding;
		ImGui::SetCursorPos({});
		_hasMouseInputCapture = detectViewportInput();
		
		if (hasContextMenu() && ImGui::BeginPopupContextWindow("Context Menu")) {
			contextMenu();
			ImGui::EndPopup();
		}
	}
	
	mtl::double2 InputSurfaceView::mouseLocationInView() const {
		return transformLocationToViewSpace(ImGui::GetIO().MousePos);
	}
	
	void InputSurfaceView::displayTexture(void const* texture) {
		ImGui::Image(const_cast<void*>(texture), this->size());
	}
	
	template<typename T>
	static constexpr bool hasOffset = requires (T&& t) { t.offset; };
	
	template<typename T>
	static constexpr bool hasLocation = requires (T&& t) {
		{ t.locationInView } /*-> std::same_as<mtl::double2>*/;
		{ t.locationInWindow } /*-> std::same_as<mtl::double2>*/;
	};
	
	template <typename E>
	void maybeTransformOffset(E& e) {
		if constexpr (hasOffset<E>) {
			using T = decltype(e.offset);
			if constexpr (mtl::get_vector_size<T>::value == 2) {
				e.offset.y *= -1.0;
			}
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
		return detectViewportInput(ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);
	}
	
	static bool detectViewportInput(ImGuiButtonFlags flags) {
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

		auto const buttonFlags = flags;
		
		bool pressed, held, hovered;
		pressed = ButtonBehavior(bb, id, &hovered, &held, buttonFlags);

		SetCursorPos(cursorPos);
		
		return pressed || held || hovered;
	}
	
}
