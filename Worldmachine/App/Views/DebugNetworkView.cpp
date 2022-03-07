#include "DebugNetworkView.hpp"

#include <imgui/imgui.h>
#include <mtl/mtl.hpp>
#include <utl/format.hpp>

#include "Framework/Window.hpp"
#include "Framework/Appearance.hpp"
#include "Framework/ImGuiExt.hpp"

#include "Core/Network/Network.hpp"

using namespace mtl;

namespace worldmachine {
	
	void DebugNetworkView::display() {
		NetworkView::display();
		
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("Debug")) {
			optionMenuItem(showColorOptions, "Color Options Panel");
			
			optionMenuItem(showPortalParameters, "Portal Parameters Panel");
			optionMenuItem(showAppearanceInspector, "Appearance Inspector Panel");
			
			optionMenuItem(showMouseMoveInfo, "Mouse Move Info");
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
		
		if (showColorOptions) {
			colorOptionsPanel();
		}
		if (showMouseMoveInfo && mouseMoveInfo) {
			mouseMoveInfo();
		}
		if (showPortalParameters) {
			portalParametersPanel();
		}
		if (showAppearanceInspector) {
			appearanceInspectPanel(getWindow()->appearance(), showAppearanceInspector);
		}
	}
	
	void DebugNetworkView::optionMenuItem(bool& b, std::string_view label) {
		if (ImGui::MenuItem(utl::format("{} {}", b ? "Hide" : "Show", label).data())) {
			b = !b;
		}
	}
	
	void DebugNetworkView::colorOptionsPanel() {
		ImGui::Begin("Color Options", &showColorOptions);
		
		for (int i = 0; i < (int)NodeCategory::count; ++i) {
			ImGui::ColorPicker4(utl::format("{}", (NodeCategory)i).c_str(), uniforms.node.colors[i].data());
		}
		
		ImGui::ColorPicker4("Unbuild", uniforms.node.unbuiltColor.data());
		ImGui::ColorPicker4("Progressbar", uniforms.node.progressBarColor.data());
		
		ImGui::DragFloat("Inner Shadow Size", &uniforms.node.innerShadowSize);
		ImGui::DragFloat("Outer Shadow Size", &uniforms.node.outerShadowSize);
		ImGui::DragFloat("Shadow Intensity", &uniforms.node.shadowIntensity, 0.01);
		ImGui::DragFloat("Selection Band Width", &uniforms.node.selectionBandWidth);
		ImGui::DragFloat("Unbuild Band Width", &uniforms.node.unbuiltBandWidth);
		ImGui::DragFloat("Progress Bar Shadow Intensity", &uniforms.node.progressbarShadowIntensity, 0.01, 0, 1);
		
		ImGui::End();
	}
	
	void DebugNetworkView::portalParametersPanel() {
		ImGui::Begin("Network View Portal Parameters", &showPortalParameters);
		
		static float4 someWorldPosition = { 0, 0, 0, 1 };
		ImGui::DragFloat2("someWorldPosition", someWorldPosition.data());
		ImGui::Text("%s", utl::format("someWorldPosition = {}", someWorldPosition).c_str());
		ImGui::Text("%s", utl::format("viewMatrix * projectionMatrix * someWorldPosition = {}", projectionMatrix * viewMatrix * someWorldPosition).c_str());
		ImGui::Text("%s", utl::format("viewMatrix * someWorldPosition = {}", viewMatrix * someWorldPosition).c_str());
		
		ImGui::Separator();
		
		if (mouseOffsetInfo)
			mouseOffsetInfo();
		
		ImGui::Separator();
		
		ImGui::DragFloat2("PortalPosition (WS)", portalParameters().position.data());
		ImGui::Text("%s", utl::format("Mouse Position (VS): {}", mouseLocation).c_str());
		ImGui::Text("%s", utl::format("View Size: {}", this->size()).c_str());
		ImGui::Text("%s", utl::format("Screen Scale Factor: {}", this->scaleFactor()).c_str());
		ImGui::Text("%s", utl::format("Portal Size: {}", uniforms.portal.size).c_str());
		float ps = portalScale;
		ImGui::DragFloat("Portal Scale", &ps, 0.01);
		portalScale = ps;
		
		
		ImGui::Separator();
		ImGui::Matrix("viewProjection = ", projectionMatrix * viewMatrix);
		ImGui::Separator();
		ImGui::Matrix("projection = ", projectionMatrix);
		ImGui::Separator();
		ImGui::Matrix("view = ", viewMatrix);
		ImGui::Separator();
		ImGui::Matrix("inverse(view) = ", inverseViewMatrix);
		
		ImGui::End();
	}
	
	void DebugNetworkView::rightMouseDragged(MouseDragEvent event) {
		NetworkView::rightMouseDragged(event);
		
		mouseOffsetInfo = [=] {
			ImGui::Text("%s", utl::format("Mouse Drag Offset: {}", event.offset).c_str());
		};
	}
	void DebugNetworkView::scrollWheel(ScrollEvent event) {
		NetworkView::scrollWheel(event);
		
		if (event.isTrackpad) {
			mouseOffsetInfo = [=] {
				ImGui::Text("%s", utl::format("Trackpad Scroll Offset: {}", event.offset).c_str());
			};
		}
	}
	void DebugNetworkView::mouseMoved(MouseMoveEvent event) {
		NetworkView::mouseMoved(event);
		
		mouseLocation = event.locationInView;
		
		auto const worldPosition = positionVStoWS(event.locationInView);
		auto const hitResult = network->hitTest(worldPosition);
		
		mouseMoveInfo = [=]{
			std::string text;
			switch (hitResult.type) {
				case NetworkHitResult::Type::node:
					text = utl::format("Node {} [position = {}, category = {}]",
									   hitResult.node.index,
									   network->nodes().get<Node::Position>(hitResult.node.index),
									   network->nodes().get<Node::Category>(hitResult.node.index));
					break;

				case NetworkHitResult::Type::background:
					text = "Background";
					break;
				case NetworkHitResult::Type::edge: {
					auto const nodeIndexFrom = network->edges().get<Edge::BeginNodeIndex>(hitResult.edge.index);
					auto const nodeIndexTo = network->edges().get<Edge::EndNodeIndex>(hitResult.edge.index);
					text = utl::format("Edge [{} -> {}]", network->nodes().get<Node::Name>(nodeIndexFrom), network->nodes().get<Node::Name>(nodeIndexTo));
					break;
				}
					
				case NetworkHitResult::Type::pin: {
					text = utl::format("Pin [{} {}]", hitResult.pin.kind, hitResult.pin.index);
					switch (hitResult.pin.kind) {
						case PinKind::input:
							
							break;
						case PinKind::output:
							break;
						case PinKind::parameterInput:
							break;
						case PinKind::maskInput:
							break;

						default:
							break;
					}
				}

				default:
					break;
			}
			
			ImGui::BeginTooltip();
			ImGui::Text("%s", text.c_str());
			ImGui::Text("%s", utl::format("mousePositionVS = {}", event.locationInView).c_str());
			ImGui::Text("%s", utl::format("mousePositionWS = {}", worldPosition).c_str());
			ImGui::EndTooltip();
		};
	}
		
}
