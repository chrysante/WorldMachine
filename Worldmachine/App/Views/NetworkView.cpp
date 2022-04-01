#include "NetworkView.hpp"

#include <imgui/imgui.h>
#include <array>
#include <optional>
#include <utl/scope_guard.hpp>

#include "Framework/Window.hpp"
#include "Framework/Typography/TypeSetter.hpp"

#include "Core/Network/Network.hpp"
#include "Core/BuildSystemFwd.hpp"

#include "App/Config.hpp"
#include "NetworkRenderer.hpp"
#include "ImageView2D.hpp"
#include "Menus.hpp"

using namespace mtl::short_types;

namespace {
	
	static auto calculateMatrices(float2 portalPosition,
								  float2 portalSize,
								  float2 viewSize,
								  float  zoomScale,
								  float  depth) {
		
		auto const scale = float4x4::diag(zoomScale, zoomScale, 1, 1);
		
		auto const view = scale * look_at(/* eye    = */ float3(portalPosition, 0),
										  /* center = */ float3(portalPosition, 1),
										  /* up     = */ float3(0, 1, 0));
		
		auto const projection = mtl::ortho(0, viewSize.x,
										   0, viewSize.y,
										   -1, depth);
		
		return std::array{ projection, view };
	}
	
	
	
}

namespace worldmachine {
	
	/// MARK: - Initialization
	NetworkView::NetworkView(Network* network):
		View("Network"),
		NodeView(network),
		network(network),
		typeSetter(utl::make_ref<TypeSetter>())
	{}

	NetworkView::~NetworkView() = default;
	
	void NetworkView::init() {
		renderer = utl::make_unique_ref<NetworkRenderer>(typeSetter);
		
		setDefaultUniformValues();
	}
	
	void NetworkView::setDefaultUniformValues() {
		FontMetrics const fontMetrics = typeSetter->metrics(defaultFont(), 64).value();
		
		uniforms.viewProjectionMatrix = {};
		uniforms.node = network->nodeParams();
		uniforms.edge = network->edgeParams();
		uniforms.portal = {
			.size = this->size(),
			.position = { 0, 0 },
			.scale = 1.0
		};
		uniforms.screenScaleFactor = this->scaleFactor();
		
		uniforms.fontMetrics = fontMetrics;
		
		uniforms.textSize = 28;
		uniforms.labelColor = {};
		
		uniforms.node.colors[(int)NodeCategory::generator] = mtl::colors<mtl::float3>::hex(0xADFE7900);
		uniforms.node.colors[(int)NodeCategory::filter]    = mtl::colors<mtl::float3>::hex(0x94DDFF00);
		uniforms.node.colors[(int)NodeCategory::natural]   = mtl::colors<mtl::float3>::hex(0xFFE7BD00);
		uniforms.node.colors[(int)NodeCategory::output]    = mtl::colors<mtl::float3>::hex(0xFFAEF100);
		uniforms.node.colors[(int)NodeCategory::parameter] = mtl::colors<mtl::float3>::hex(0xFFFFFF00);
		uniforms.node.unbuiltColor                         = mtl::colors<mtl::float3>::hex(0xFF767600);
		uniforms.node.progressBarColor                     = mtl::colors<mtl::float3>::hex(0xACFFAE00);
		uniforms.node.innerShadowSize                      = 25;
		uniforms.node.outerShadowSize                      = 50;
		uniforms.node.shadowIntensity                      = 0.25;
		uniforms.node.progressbarShadowIntensity           = 0.2;
	}
	
	
	void NetworkView::onFileOpen() {
		double2 const centerOfMass = [&]{
			double2 result = 0;
			for (auto const& position: network->nodes.view<Node::members::position const>()) {
				result += position.xy;
			}
			return result / network->nodeCount();
		}();
		portalParameters().position = centerOfMass - this->size() / 2;
	}
	
	
	/// MARK: - Display
	void NetworkView::display() {
		portalParameters().size = size();
		uniforms.screenScaleFactor = this->scaleFactor();
		portalParameters().size = this->size() / portalScale;
		portalParameters().scale =  portalScale;
		std::tie(projectionMatrix, viewMatrix) = calculateMatrices(portalParameters().position,
																   portalParameters().size,
																   this->size(),
																   portalScale,
																   network->nodeCount());
		inverseViewMatrix = mtl::inverse(viewMatrix);
		uniforms.viewProjectionMatrix = mtl::transpose(projectionMatrix * viewMatrix);
		uniforms.nodeCount = network->nodeCount();
		
		Appearance const& a = getWindow()->appearance();
		
		
		uniforms.backgroundColor = a.windowBackgroundColor;  // re2::controlBackgroundColor();
		uniforms.labelColor      = a.labelColor;     // re2::labelColor();
		uniforms.edgeColor       = a.labelColor;   // re2::labelColor();
		uniforms.lineColor       = a.gridColor;   // re2::disabledControlTextColor();
		uniforms.shadowOffset    = { 0, 0 };
		renderer->draw(*this,
					   *network,
					   uniforms,
					   draggingEdge,
					   selectionRectangle);
		

		ImGui::Image(renderer->renderedImage(), this->size());
		
		if (tooltip) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5, 5 });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3);
			ImGui::BeginTooltip();
			tooltip();
			ImGui::EndTooltip();
			ImGui::PopStyleVar(2);
		}
	}
	

	/// MARK: - Context Menus
	void NetworkView::backgroundContextMenu() {
		if (createNodesMenu.display()) {
			NodeDescriptor desc = createNodesMenu.selectedElement();
			desc.position.xy = menuOpenedAtWS;
			network->locked([&]{
				network->addNode(desc);
			});
		}
	}
	
	void NetworkView::nodeContextMenu(std::size_t nodeIndex) {
		auto* const nodeImpl = network->nodes[nodeIndex].implementation.get();
		if (nodeImpl == NodeView::getActiveDisplayNodeImplementationPointer()) {
			if (ImGui::MenuItem("Unlock Display")) {
				setActiveDisplayNode(std::nullopt);
				ImGui::CloseCurrentPopup();
			}
		}
		else {
			if (ImGui::MenuItem("Lock Display")) {
				setActiveDisplayNode(nodeIndex);
				ImGui::CloseCurrentPopup();
			}
		}
		
		
		/// Deletion
		auto eitherWay = [&]{
			if (getActiveNodeIndex() == nodeIndex) {
				setActiveNode(std::nullopt);
			}
			if (getActiveDisplayNodeIndex() == nodeIndex) {
				setActiveDisplayNode(std::nullopt);
			}
		};
		if (network->isSelected(nodeIndex)) {
			std::string_view text = network->selectedIndices().size() > 1 ? "Delete Nodes" : "Delete Node";
			if (ImGui::MenuItem(text.data())) {
				eitherWay();
				network->locked([&]{
					network->removeSelectedNodes();
				});
				ImGui::CloseCurrentPopup();
			}
		}
		else {
			if (ImGui::MenuItem("Delete Node")) {
				eitherWay();
				network->locked([&]{
					network->removeNode(nodeIndex);
				});
				ImGui::CloseCurrentPopup();
			}
		}
	}
	
	void NetworkView::edgeContextMenu(std::size_t edgeIndex) {
		if (ImGui::MenuItem("Delete Edge")) {
			network->removeEdge(edgeIndex);
		}
	}
	
	
	/// MARK: - Set Active Node
	void NetworkView::setActiveNode_Impl(std::optional<utl::UUID> id, auto setter) {
		getWindow()->forEachView([&](View* view) {
			auto* nodeView = dynamic_cast<NodeView*>(view);
			if (!nodeView) {
				return;
			}
			(nodeView->*setter)(id);
		});
	}
	
	void NetworkView::setActiveNode(std::optional<std::size_t> nodeIndex) {
		setActiveNode(nodeIndex ? std::optional(network->IDFromIndex(*nodeIndex)) : std::nullopt);
	}
	
	void NetworkView::setActiveNode(std::optional<utl::UUID> id) {
		setActiveNode_Impl(id, &NodeView::setActiveNode);
	}
	
	void NetworkView::setActiveNode(std::nullopt_t) {
		setActiveNode_Impl(std::nullopt, &NodeView::setActiveNode);
	}
	
	void NetworkView::setActiveDisplayNode(std::optional<std::size_t> nodeIndex) {
		setActiveDisplayNode(nodeIndex ? std::optional(network->IDFromIndex(*nodeIndex)) : std::nullopt);
	}
	
	void NetworkView::setActiveDisplayNode(std::optional<utl::UUID> id) {
		setActiveNode_Impl(id, &NodeView::setActiveDisplayNode);
	}
	
	void NetworkView::setActiveDisplayNode(std::nullopt_t) {
		setActiveNode_Impl(std::nullopt, &NodeView::setActiveDisplayNode);
	}
	
	/// MARK: - Input
	bool NetworkView::hasContextMenu() const {
		return !!contextMenuFn;
	}
	
	void NetworkView::contextMenu() {
		contextMenuFn();
	}
	
	
	static SelectOperation chooseRectangleSelectOperation(EventModifierFlags flags) {
		if (!!(flags & EventModifierFlags::super)) {
			return SelectOperation::setSymmetricDifference;
		}
		else if (!!(flags & EventModifierFlags::shift)) {
			return SelectOperation::setUnion;
		}
		else
			return SelectOperation::setUnion; // doesnt matter, because selection gets cleared before
	}
	
	void NetworkView::handleBackgroundClick(MouseDownEvent event) {
		setActiveNode(std::nullopt);
		
		if (!(event.modifierFlags & EventModifierFlags::super) && !(event.modifierFlags & EventModifierFlags::shift)) {
			network->clearSelection();
		}
	
		auto const selectOperation = chooseRectangleSelectOperation(event.modifierFlags);
		selectionRectangle = SelectionRectangle(positionVStoWS(event.locationInView));

		_mouseDragged = [=](MouseDragEvent event) {
			selectionRectangle->addOffset(event.offset / portalScale);
			network->setRectangleSelection(selectionRectangle->get(), selectOperation);
		};

		_mouseUp = [=](MouseEvent event) {
								   selectionRectangle = std::nullopt;
			network->applyRectangleSelection(selectOperation);
			if (network->selectedIndices().size() == 1) {
				setActiveNode(network->selectedIndices().front());
			}
		};
	}
	
	void NetworkView::handleEdgeClick(MouseDownEvent event, std::size_t edgeIndex) {
		if (event.clickCount >= 2) {
			removeEdge(edgeIndex);
		}
	}
	
	void NetworkView::handleNodeClick(MouseDownEvent event, std::size_t nodeIndex) {
		if /** Node is already selected */ (network->isSelected(nodeIndex)) {
			if /** Command key is not pressed */ (!(event.modifierFlags & EventModifierFlags::super)) {
				_mouseUp = [=](auto const& event) {
					network->clearSelection();
					network->selectNode(nodeIndex);
					setActiveNode(nodeIndex);
				};
			}
			else /** Command key is pressed */ {
				_mouseUp = [=](MouseEvent event) {
					network->unselectNode(nodeIndex);
				};
			}
		}
		else /** Node is not selected yet */ {
			if /** Command key is not pressed */ (!(event.modifierFlags & EventModifierFlags::super)) {
				network->clearSelection();
			}
			/**
			 Deliberately select and make active right right away instead of onMouseUp.
			 This behaviour mimics the OS and feels natural.
			 */
//			network->locked([&]{
				network->selectNode(nodeIndex);
				network->moveToTop(nodeIndex);
//			});
			setActiveNode(nodeIndex);
		}

		_mouseDragged = [this](MouseDragEvent event) {
			network->moveSelected(directionVStoWS(event.offset));
			_mouseUp = nullptr;
		};
	}
	
	void NetworkView::handleNodeRightClick(MouseDownEvent event, std::size_t nodeIndex) {
		if /** Command key is not pressed */ (!(event.modifierFlags & EventModifierFlags::super)) {
			if (network->selectedIndices().size() < 2 && !network->isSelected(nodeIndex)) {
				WM_Log("Selecting node {}", network->nodes[nodeIndex].name);
				network->selectNode(nodeIndex);
			}
		}
	}
	
	void NetworkView::handlePinClickImageNode(MouseDownEvent event,
													NetworkHitResult const& hitResult) {
		WM_Expect(hitResult.type == NetworkHitResult::Type::pin, "hitResult is of wrong type");
//		WM_Expect(hitResult.pin.has_value(), "'pin' was empty");
//		WM_Expect(hitResult.node.has_value(), "'node' was empty");

		beginEdgeDragging(event, hitResult);
	}
	
	void NetworkView::beginEdgeDragging(MouseEvent event,
										NetworkHitResult const& hitResult) {
		draggingEdge = EdgeProxy{
			.begin = hitResult.pin.position,
			.end = positionVStoWS(event.locationInView),
			.beginIndex = (float)hitResult.node.index,
			.endIndex = (float)network->nodeCount()
		};

		_mouseDragged = [this, draggingFrom = hitResult.pin.kind](MouseDragEvent event) {
			draggingEdge->end = positionVStoWS(event.locationInView);
			auto const wsPosition = positionVStoWS(event.locationInView);
			auto const hitResult = network->hitTest(wsPosition);
			setTooltip(hitResult, draggingFrom);
		};

		_mouseUp = [this, beginHitResult = hitResult](MouseEvent const& event) {
			auto const endHitResult = network->hitTest(positionVStoWS(event.locationInView));
			draggingEdge = std::nullopt;

			if (endHitResult.type == NetworkHitResult::Type::pin) {
				if (network->isBuilding()) {
					WM_Log(error, "Can't add edge while building");
				}
				else {
					network->tryAddEdge({
						.nodeIndex      = beginHitResult.node.index,
						.pinIndex = beginHitResult.pin.index,
						.pinKind  = beginHitResult.pin.kind,
					}, {
						.nodeIndex      = endHitResult.node.index,
						.pinIndex = endHitResult.pin.index,
						.pinKind  = endHitResult.pin.kind
					});
				}
			}
		};
	}
	
	void NetworkView::mouseDown(MouseDownEvent event) {
		auto const wsPosition = positionVStoWS(event.locationInView);
		auto hitResult = network->hitTest(wsPosition);
		
		switch (hitResult.type) {
			case NetworkHitResult::Type::background:
				handleBackgroundClick(event);
				break;

			case NetworkHitResult::Type::node:
				handleNodeClick(event, hitResult.node.index);
				break;

			case NetworkHitResult::Type::pin:
				if (hitResult.node.category == NodeCategory::parameter) {
					WM_DebugBreak("implement click on parameter node");
				}
				else /* imageNode */ {
					handlePinClickImageNode(event, hitResult);
				}
				break;

			case NetworkHitResult::Type::edge:
				handleEdgeClick(event, hitResult.edge.index);
				break;

			default:
				WM_DebugBreak("Click event not handled");
		}
	}

	void NetworkView::mouseDragged(MouseDragEvent event) {
		if (_mouseDragged)
			_mouseDragged(event);
	}
	
	void NetworkView::mouseUp(MouseUpEvent event) {
		if (_mouseUp)
			_mouseUp(event);

		_mouseUp = nullptr;
		_mouseDragged = nullptr;
	}

	
	void NetworkView::rightMouseDown(MouseDownEvent event) {
//		auto const position = positionVStoWS(event.locationInView);
//		auto const hitResult = network->hitTest(position);
//		
//		switch (hitResult.type) {
//			case NetworkHitResult::Type::node:
//				handleNodeRightClick(event, hitResult.node.index);
//				break;
//				
//			default:
//				break;
//		}
		
		_setContextMenu = [this](MouseUpEvent const event) {
			auto const position = positionVStoWS(event.locationInView);
			auto const hitResult = network->hitTest(position);
			switch (hitResult.type) {
				case NetworkHitResult::Type::node:
					contextMenuFn = [=] {
						nodeContextMenu(hitResult.node.index);
					};
					break;
				case NetworkHitResult::Type::edge:
					contextMenuFn = [=] {
						edgeContextMenu(hitResult.edge.index);
					};
					break;
				case NetworkHitResult::Type::background:
					contextMenuFn = [=] {
						backgroundContextMenu();
					};
					menuOpenedAtWS = position;
					break;
				default:
					break;
			}
		};
	}
	
	void NetworkView::rightMouseDragged(MouseDragEvent event) {
		portalParameters().position -= event.offset / portalScale;
		_setContextMenu = [this](auto){ contextMenuFn = nullptr; };
	}
	
	void NetworkView::rightMouseUp(MouseUpEvent event) {
		if (_setContextMenu)
			_setContextMenu(event);
	}
	
	void NetworkView::scrollWheel(ScrollEvent event) {
		if (event.isTrackpad) {
			portalParameters().position -= event.offset / portalScale;
		}
		else {
			zoom(-utl::signed_pow(event.offset.y, 2.0/3.0) / 100, event.locationInView);
		}
	}
	
	void NetworkView::magnify(MagnificationEvent event) {
		zoom(event.offset, event.locationInView);
	}
	
	void NetworkView::mouseMoved(MouseMoveEvent event) {
		auto const worldPosition = positionVStoWS(event.locationInView);
		auto const hitResult = network->hitTest(worldPosition);
		
		setTooltip(hitResult);
	}
	
	void NetworkView::zoom(float offset, double2 mousePositionVS) {
		// new scale
		double const magnification = offset * portalScale;
		double const newScale = portalScale + magnification;
		double const newScaleClamped = std::clamp(newScale, 0.2, 25.0);
		
		// position offset
		double const clampEffect = newScale - newScaleClamped;
		double const effectiveMagnification = magnification - clampEffect;
		double const positionOffsetFactor = effectiveMagnification / portalScale / newScaleClamped;
		double2 const portalPositionOffset = mousePositionVS * positionOffsetFactor;
		
		portalScale = newScaleClamped;
		portalParameters().position += portalPositionOffset;
	}
	
	
	bool NetworkView::keyDown(KeyEvent event) {
		switch (event.keyCode) {
			case KeyCode::X:
				if (!network->selectedIndices().empty()) {
					network->removeSelectedNodes();
					return true;
				}
				break;
			case KeyCode::L:
				if (network->selectedIndices().size() == 1) {
					std::size_t const nodeIndex = network->selectedIndices().front();
					setActiveDisplayNode(getActiveDisplayNodeIndex() != nodeIndex ? std::optional(nodeIndex) : std::nullopt);
					return true;
				}
				else if (network->selectedIndices().empty()) {
					setActiveDisplayNode(std::nullopt);
					return true;
				}
				break;
			default:
				break;
		}
		return false;
	}
	
	void NetworkView::setTooltip(NetworkHitResult const& hitResult,
								 std::optional<PinKind> const& draggingFrom)
	{
		switch (hitResult.type) {
			case NetworkHitResult::Type::pin: {
				auto const& descArray = network->nodes[hitResult.node.index].pinDescriptorArray;
				auto const& name = descArray.get(hitResult.pin.kind)[hitResult.pin.index].name();
				std::string text = [&]{
					if (draggingFrom.has_value()) {
						bool const isCompatible = isCompatibleForImageNode(*draggingFrom,
																		   hitResult.pin.kind);
						return utl::format("{} {}", name, isCompatible ? "" : "[Incompatible]");
					}
					else {
						return utl::format("{}", name);
					}
				}();
				
				tooltip = [text = std::move(text)] {
					ImGui::Text("%s", text.data());
				};
				break;
			}
			default:
				tooltip = nullptr;
				break;
		}
	}
	
	
	void NetworkView::removeEdge(std::size_t index) const {
		if (network->isBuilding()) {
			WM_Log(error, "Can't remove Edge while building");
			return;
		}
		network->removeEdge(index);
	}

	void NetworkView::setTooltip(NetworkHitResult const& hitResult) {
		setTooltip(hitResult, std::nullopt);
	}
	
	mtl::double2 NetworkView::positionVStoWS(mtl::double2 p) const {
		return (inverseViewMatrix * double4(p, 0, 1)).xy;
	}
	
	mtl::double2 NetworkView::directionVStoWS(mtl::double2 d) const {
		return mtl::dimension_cast<2, 2>(inverseViewMatrix) * d;
	}

}


