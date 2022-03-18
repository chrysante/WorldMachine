#pragma once

#include <optional>
#include <string>
#include <utl/memory.hpp>
#include <Metal/Metal.hpp>

#include "Framework/Platform/MacOS/MTLARCPointer.hpp"
#include "Framework/Typography/TextRenderer.hpp"
#include "Framework/Typography/TextRenderData.hpp"
#include "Framework/Renderer.hpp"
#include "Framework/View.hpp"

#include "SharedNetworkViewTypes.hpp"
#include "SelectionRectangle.hpp"

namespace worldmachine {
	
	class Network;
	class TypeSetter;
	
	using NodeNameRenderData = std::array<LetterData, nodeNameMaxRenderSize>;
	
	class NetworkRenderer: public Renderer {
	public:
		NetworkRenderer(utl::ref<TypeSetter>);
		
		void draw(View&,
				  Network const&,
				  NetworkUniforms const&,
				  std::optional<EdgeProxy> draggingEdge,
				  std::optional<SelectionRectangle> selection);
		
	private:
		void drawDraggingEdge(EdgeProxy, MTL::RenderCommandEncoder*);
		void drawEdges(Network const&, MTL::RenderCommandEncoder*);
		void drawEdges_impl(std::size_t count,
							MTL::Buffer* edgeDataBuffer,
							MTL::RenderCommandEncoder*);
		void drawNodes(Network const&, MTL::RenderCommandEncoder*);
		void drawNodeShadows(Network const&, MTL::RenderCommandEncoder*);
		void drawSelection(SelectionRectangle, MTL::RenderCommandEncoder*);
		void drawNames(Network const&, Font, float textSize, float screenScale, MTL::RenderCommandEncoder*);
		void drawNameFrames(Network const&, MTL::RenderCommandEncoder*);
		void drawBackground(MTL::RenderCommandEncoder*);
		
		void drawLabels(MTL::RenderCommandEncoder*);
		
		void createNodeBuffers(std::size_t nodeCount);
		void createEdgeProxyBuffer(std::size_t edgeCount);
		
		void updateNodeBuffers(Network const&);
		void createNodeNameRenderData(Network const&);
		
	private:
		MTLARCPointer<MTL::DepthStencilState>   depthStencilStateNone;
		MTLARCPointer<MTL::DepthStencilState>   depthStencilStateOpaque;
		MTLARCPointer<MTL::DepthStencilState>   depthStencilStateTranslucent;
		// nodes
		MTLARCPointer<MTL::RenderPipelineState> nodePipelineState;
		MTLARCPointer<MTL::RenderPipelineState> nodeShadowPipelineState;
		MTLARCPointer<MTL::Buffer>              nodePositionBuffer;
		MTLARCPointer<MTL::Buffer>              nodeSizeBuffer;
		MTLARCPointer<MTL::Buffer>              nodeCategoryBuffer;
		MTLARCPointer<MTL::Buffer>              nodeFlagsBuffer;
		MTLARCPointer<MTL::Buffer>              nodePinCountBuffer;
		MTLARCPointer<MTL::Buffer>              nodeBuildProgressBuffer;
		MTLARCPointer<MTL::Buffer>              nodeNameBuffer;
		std::size_t                             nodeBufferSize = 0;
		MTLARCPointer<MTL::RenderPipelineState> nodeNamePipelineState;
		
		utl::vector<NodeNameRenderData> nodeNameRenderData;
		
		// selectionRectangle
		MTLARCPointer<MTL::RenderPipelineState> selectionPipelineState;
		MTLARCPointer<MTL::Buffer>              selectionDataBuffer;
		
		// edgeProxy
		MTLARCPointer<MTL::RenderPipelineState> edgePipelineState;
		MTLARCPointer<MTL::Buffer>              draggingEdgeBuffer;
		MTLARCPointer<MTL::Buffer>              edgeProxyBuffer;
		std::size_t                             edgeProxyBufferSize = 0;
		
		// background
		MTLARCPointer<MTL::RenderPipelineState> backgroundPipelineState;
		
		// uniforms
		MTLARCPointer<MTL::Buffer>              uniformBuffer;
		MTLARCPointer<MTL::Buffer>              nodeNameRenderDataBuffer;
				
		utl::ref<TextRenderer> textRenderer;
		utl::ref<TypeSetter>   typeSetter;
	};

}
