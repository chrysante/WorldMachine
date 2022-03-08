#pragma once

#include "Framework/View.hpp"

#include <optional>
#include <utl/functional.hpp>
#include <utl/UUID.hpp>
#include <utl/memory.hpp>

#include "NodeView.hpp"
#include "SelectionRectangle.hpp"
#include "SharedNetworkViewTypes.hpp"
#include "Menus.hpp"


namespace worldmachine {
	
	class Network;
	struct NetworkHitResult;
	struct PinIndex;
	class NetworkRenderer;
	class BuildSystem;
	
	class NetworkView: public InputSurfaceView, public NodeView {
		friend class DebugNetworkView;
		
	public:
		NetworkView(Network*);
		~NetworkView();
		
		void onFileOpen();
	
	private:
		void init() override;
		void display() override;
		
	/// MARK: Input
		void mouseDown(MouseDownEvent) override;
		void mouseDragged(MouseDragEvent) override;
		void mouseUp(MouseUpEvent) override;
		
		void rightMouseDown(MouseDownEvent) override;
		void rightMouseDragged(MouseDragEvent) override;
		void rightMouseUp(MouseUpEvent) override;
		
		void scrollWheel(ScrollEvent) override;
		void magnify(MagnificationEvent) override;
		void mouseMoved(MouseMoveEvent) override;
		
		void handleBackgroundClick(MouseDownEvent);
		void handleEdgeClick(MouseDownEvent, std::size_t edgeIndex);
		void handleNodeClick(MouseDownEvent, std::size_t nodeIndex);
		void handleNodeRightClick(MouseDownEvent, std::size_t nodeIndex);
		void handlePinClickImageNode(MouseDownEvent,
										   NetworkHitResult const&);
		void handlePinClickParameterNode(MouseDownEvent,
											   NetworkHitResult const&);
		void beginEdgeDragging(MouseEvent,
							   NetworkHitResult const&);
		void zoom(float offset, mtl::double2 mousePositionVS);
		
		bool keyDown(KeyEvent) override;
		
		utl::function<void(MouseUpEvent)> _mouseUp;
		utl::function<void(MouseUpEvent)> _rightMouseUp;
		utl::function<void(MouseDragEvent)> _mouseDragged;
		utl::function<void(MouseDragEvent)> _rightMouseDragged;
		
		utl::function<void(MouseUpEvent)> _setContextMenu;
		void setTooltip(NetworkHitResult const&,
						std::optional<PinKind> const& draggingFrom);
		void setTooltip(NetworkHitResult const&);
		
	private:
		void removeEdge(std::size_t index) const;
		
	private:
		void setDefaultUniformValues();
		
	private:
		PortalParameters& portalParameters() { return uniforms.portal; }
		PortalParameters const& portalParameters() const { return uniforms.portal; }
		
		[[nodiscard]] mtl::double2 positionVStoWS(mtl::double2) const;
		[[nodiscard]] mtl::double2 directionVStoWS(mtl::double2) const;
	
	private:
		utl::function<void()> contextMenu;
		utl::function<void()> tooltip;
		
		void backgroundContextMenu();
		void nodeContextMenu(std::size_t nodeIndex);
		void edgeContextMenu(std::size_t edgeIndex);
	
	private:
		void setActiveNode(std::optional<std::size_t> nodeIndex);
		void setActiveNode(std::optional<utl::UUID> nodeID);
		void setActiveNode(std::nullopt_t);
		
		void setActiveDisplayNode(std::optional<std::size_t> nodeIndex);
		void setActiveDisplayNode(std::optional<utl::UUID> nodeID);
		void setActiveDisplayNode(std::nullopt_t);

		void setActiveNode_Impl(std::optional<utl::UUID> nodeID, auto);
		
	private:
		Network* network;
		utl::unique_ref<NetworkRenderer> renderer;
		NetworkUniforms uniforms{};
		mtl::double4x4 projectionMatrix, viewMatrix, inverseViewMatrix;
		double portalScale = 1;
		std::optional<SelectionRectangle> selectionRectangle;
		std::optional<EdgeProxy> draggingEdge = std::nullopt;
		CreateNodesMenu createNodesMenu;
		mtl::double2 menuOpenedAtWS = 0;
	};
	
}



