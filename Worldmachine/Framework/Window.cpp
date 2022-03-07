#define IMGUI_DEFINE_MATH_OPERATORS
#define UTL_DEFER_MACROS 1

#include "Window.hpp"

#include <mtl/mtl.hpp>
#include <utl/functional.hpp>
#include <utl/scope_guard.hpp>
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

#include "Core/Debug.hpp"

using namespace mtl::short_types;

#if 0
const float toolbarSize = 50;
float menuBarHeight;
#endif

static void DockSpaceUI(bool hasToolbar /* unused */) {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	auto nextWindowPos = viewport->Pos;
	auto nextWindowSize = viewport->Size;
#if 0
	if (hasToolbar) {
		nextWindowPos.y += toolbarSize;
		nextWindowSize.y -= toolbarSize;
	}
#endif
	
	ImGui::SetNextWindowPos(nextWindowPos);
	ImGui::SetNextWindowSize(nextWindowSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("Master DockSpace", NULL, window_flags);
	ImGuiID dockMain = ImGui::GetID("MyDockspace");

//	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	auto const winSizeSaved = style.WindowMinSize.x;
	style.WindowMinSize.x = 250;
	ImGui::DockSpace(dockMain);
	style.WindowMinSize.x = winSizeSaved;
	ImGui::End();
	ImGui::PopStyleVar(3);
}

#if 0
static void ToolbarUI() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings
		;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("TOOLBAR", NULL, window_flags);
	ImGui::PopStyleVar();



	ImGui::Button("Toolbar goes here", ImVec2(0, 37)); ImGui::SameLine();
	ImGui::Button("Toolbar goes here", ImVec2(0, 37)); ImGui::SameLine();
	ImGui::Button("Toolbar goes here", ImVec2(0, 37)); ImGui::SameLine();



	ImGui::End();
}
#endif

namespace worldmachine {
	
	Window::Window(std::string title):
		_title(title)
	{
		_setStyle();
	}
	
	Window::~Window() {
		
	}
	
	View* Window::findViewByName(std::string_view name) {
		return utl::as_mutable(utl::as_const(*this).findViewByName(name));
	}
	
	View const* Window::findViewByName(std::string_view name) const {
		for (auto& v: _views) {
			if (v->name() == name) {
				return v.get();
			}
		}
		return nullptr;
	}
	
	void Window::_doDisplay() {
		if (_appearance.type != internal::getCurrentAppearanceType()) {
			_setStyle();
		}
		
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
		utl_defer { ImGui::PopStyleVar(1); };
		
		DockSpaceUI(false);

		this->_size = ImGui::GetIO().DisplaySize;
		
		// display ourself
		this->display();
		
		// display all our views
		for (auto& v: _views) {
			v->_doDisplay();
		}
	}
	
	
	void Window::_handleMouseEvent(EventUnion event) {
		event.mouseEvent.locationInWindow = _transformLocationAppKitToImGui(event.mouseEvent.locationInWindow);
		for (auto& _v: _views) {
			InputSurfaceView* v = dynamic_cast<InputSurfaceView*>(_v.get());
			if (v && v->_hasMouseInputCapture) {
				v->_handleEvent(event);
				break;
			}
		}
	}
	
	bool Window::_handleKeyEvent(KeyEvent event, bool isDown) {
		
		if (isDown) {
			if (this->keyDown(event)) {
				return true;
			}
		}
		else {
			if (this->keyUp(event)) {
				return true;
			}
		}
		
		for (auto& _v: _views) {
			if (!_v->isFocused()) {
				continue;
			}
			if (_v->_handleKeyEvent(event, isDown)) {
				return true;
			}
		}
		return false;
	}
	
	mtl::double2 Window::_transformLocationAppKitToImGui(mtl::double2 location) const {
		location.y = _size.y - location.y;
		return location;
	}
	
	mtl::double2 Window::_transformDirectionAppKitToImGui(mtl::double2 direction) const {
		direction.y = -direction.y;
		return direction;
	}
	
	
	void Window::_setStyle() {
		_appearance = Appearance::getCurrent();
		setImGuiStyleColors(_appearance);
		if (_invalidate)
			_invalidate(3);
	}
	
	
	void Window::addView(View* view) {
		view->_window = this;
		view->init();
		_views.emplace_back(view);
	}
	
	
}

