#include "NetworkRenderer.hpp"

#include "Framework/Vertex2D.hpp"
#include "Framework/Renderer.hpp"
#include "Core/Network/Network.hpp"

using namespace MTL;
using namespace worldmachine;
using namespace mtl::short_types;

namespace {
	
	std::array constexpr glyphAtlasSizes = { 8, 16, 32, 64, 128, 256 };
	
}

namespace worldmachine {
	
	NetworkRenderer::NetworkRenderer(utl::ref<TypeSetter> typeSetter):
		typeSetter(typeSetter)
	{
		useDepthBuffer = true;
		/// TODO: expose this in the API
		MSAA = 1;
		textRenderer = utl::make_ref<TextRenderer>(typeSetter);
		
		MTLARCPointer dsDesc = DepthStencilDescriptor::alloc()->init();
		dsDesc->setDepthWriteEnabled(false);
		dsDesc->setDepthCompareFunction(CompareFunctionAlways);
		depthStencilStateNone = device->newDepthStencilState(dsDesc.get());
		
		dsDesc->setDepthWriteEnabled(true);
		dsDesc->setDepthCompareFunction(CompareFunctionLessEqual);
		depthStencilStateOpaque = device->newDepthStencilState(dsDesc.get());
		
		dsDesc->setDepthWriteEnabled(false);
		dsDesc->setDepthCompareFunction(CompareFunctionLessEqual);
		depthStencilStateTranslucent = device->newDepthStencilState(dsDesc.get());
		
		MTLARCPointer lib = device->newDefaultLibrary();
		
		
		// node
		{
			
			
			auto const desc = defaultPipelineDescriptor(lib.get(),
														"nodeVertexShader",
														"nodeFragmentShader");
			NS::Error* errors;
			nodePipelineState = device->newRenderPipelineState(desc.get(), &errors);
		}
		{
			auto const desc = translucencyPipelineDescriptor(lib.get(),
															 "nodeShadowVertexShader",
															 "nodeShadowFragmentShader");
			NS::Error* errors;
			nodeShadowPipelineState = device->newRenderPipelineState(desc.get(), &errors);
		}
		
		nodeNamePipelineState = textRenderer->createPipelineState(lib.get(),
																  "nodeNameVertexShader",
																  "nodeNameFragmentShader");
		
		// selection
		{
			auto const desc = translucencyPipelineDescriptor(lib.get(),
															 "selectionVertexShader",
															 "selectionFragmentShader");
			NS::Error* errors;
			selectionPipelineState = device->newRenderPipelineState(desc.get(), &errors);
		}
		selectionDataBuffer = device->newBuffer(sizeof(mtl::rectangle<float>), ResourceStorageModeManaged);
		
		// edge
		{
			auto const desc = Renderer::defaultPipelineDescriptor(lib.get(),
																  "edgeVertexShader",
																  "edgeFragmentShader");
			NS::Error* errors;
			edgePipelineState = device->newRenderPipelineState(desc.get(), &errors);
		}
		draggingEdgeBuffer = device->newBuffer(sizeof(EdgeProxy), ResourceStorageModeManaged);
		
		// background
		{
			auto desc = Renderer::defaultPipelineDescriptor(lib.get(),
															"nodeMapBackgroundVertexShader",
															"nodeMapBackgroundFragmentShader");
			NS::Error* errors;
			backgroundPipelineState = device->newRenderPipelineState(desc.get(), &errors);
		}
		
		uniformBuffer = device->newBuffer(sizeof(NetworkUniforms), ResourceStorageModeManaged);
		nodeNameRenderDataBuffer = device->newBuffer(sizeof(LabelRenderData), ResourceStorageModeManaged);
	}
	
	void NetworkRenderer::createNodeBuffers(std::size_t nodeCount) {
		nodePositionBuffer =  device->newBuffer((uint32_t)nodeCount * sizeof(Node::Position),       ResourceStorageModeManaged);
		nodeSizeBuffer     =  device->newBuffer((uint32_t)nodeCount * sizeof(Node::Size),           ResourceStorageModeManaged);
		nodeCategoryBuffer =  device->newBuffer((uint32_t)nodeCount * sizeof(NodeCategory),         ResourceStorageModeManaged);
		nodeFlagsBuffer    =  device->newBuffer((uint32_t)nodeCount * sizeof(NodeFlags),            ResourceStorageModeManaged);
		nodePinCountBuffer
						   =  device->newBuffer((uint32_t)nodeCount * sizeof(PinCount<>),     ResourceStorageModeManaged);
		nodeBuildProgressBuffer
		                   =  device->newBuffer((uint32_t)nodeCount * sizeof(Node::BuildProgress),  ResourceStorageModeManaged);
		nodeNameBuffer     =  device->newBuffer((uint32_t)nodeCount * sizeof(NodeNameRenderData), ResourceStorageModeManaged);
		
	}
	
	void NetworkRenderer::createEdgeProxyBuffer(std::size_t edgeCount) {
		edgeProxyBuffer = device->newBuffer((uint32_t)edgeCount * sizeof(Edge::Proxy), ResourceStorageModeManaged);
	}
	
	void NetworkRenderer::draw(View& view,
							   Network const& network,
							   NetworkUniforms const& uniforms,
							   std::optional<EdgeProxy> draggingEdge,
							   std::optional<SelectionRectangle> selection) {
		
		auto* const commandBuffer = commandQueue->commandBuffer();
		
		updateRenderTargets(view.size() * view.scaleFactor());
		
		fillBuffer(uniformBuffer.get(), uniforms);
		
		MTLARCPointer renderPassDesc = RenderPassDescriptor::alloc()->init();
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderTexture.get());
		renderPassDesc->colorAttachments()->object(0)->setLoadAction(LoadActionClear);
		renderPassDesc->colorAttachments()->object(0)->setClearColor(ClearColor(0.2, 0.2, 0.2, 1));
		renderPassDesc->colorAttachments()->object(0)->setStoreAction(StoreActionStore);
		
		renderPassDesc->depthAttachment()->setTexture(depthTexture.get());
		renderPassDesc->depthAttachment()->setLoadAction(LoadActionClear);
		renderPassDesc->depthAttachment()->setClearDepth(1);
		renderPassDesc->depthAttachment()->setStoreAction(StoreActionStore);
		
		if (!renderPassDesc) {
			WM_Log(error, "RenderPassDescriptor invalid");
			WM_DebugBreak("RenderPassDescriptor invalid");
		}

		auto* const renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDesc.get());
		
		// fill node Buffers
//		if (nodeBufferSize < network.nodeCount()) {
//			createNodeBuffers(network.nodeCount());
//			nodeBufferSize = network.nodeCount();
//		}
		
		updateNodeBuffers(network);
		
//		if (!network.nodes().empty()) {
//			auto copyNodeBuffer = [&]<typename T>(Buffer* buffer) {
//				worldmachine::fillBuffer(buffer, network.nodes().data<T>(), network.nodeCount() * sizeof(T));
//			};
//			copyNodeBuffer.operator()<Node::Position>(nodePositionBuffer.get());
//			copyNodeBuffer.operator()<Node::Size>(nodeSizeBuffer.get());
//			copyNodeBuffer.operator()<NodeCategory>(nodeCategoryBuffer.get());
//			copyNodeBuffer.operator()<Node::Flags>(nodeFlagsBuffer.get());
//			copyNodeBuffer.operator()<PinCount<>>(nodePinCountBuffer.get());
//			copyNodeBuffer.operator()<Node::BuildProgress>(nodeBuildProgressBuffer.get());
//			copyNodeBuffer.operator()<NameRenderData>(nodeNameBuffer.get());
//		}
		// fill edge Buffer
		if (edgeProxyBufferSize < network.edgeCount()) {
			createEdgeProxyBuffer(network.edgeCount());
			edgeProxyBufferSize = network.edgeCount();
		}
		if (!network.edges().empty()) {
			worldmachine::fillBuffer(edgeProxyBuffer.get(),
									 network.edges().data<Edge::Proxy>(),
									 network.edgeCount() * sizeof(Edge::Proxy));
		}
		drawBackground(renderCommandEncoder);

		if (draggingEdge) {
			drawDraggingEdge(*draggingEdge, renderCommandEncoder);
		}

		drawEdges(network, renderCommandEncoder);
		drawNodes(network, renderCommandEncoder);
		drawNodeShadows(network, renderCommandEncoder);
		drawNames(network, { FontWeight::regular, FontStyle::roman },
				  uniforms.textSize / std::sqrt(uniforms.portal.scale), (uniforms.screenScaleFactor * uniforms.portal.scale).x,
				  renderCommandEncoder);

		if (selection) {
			drawSelection(*selection, renderCommandEncoder);
		}

		renderCommandEncoder->endEncoding();

		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
		
	}
	
	void NetworkRenderer::drawDraggingEdge(EdgeProxy edge, RenderCommandEncoder* commandEncoder) {
		fillBuffer(draggingEdgeBuffer.get(), edge);

		drawEdges_impl(1, draggingEdgeBuffer.get(), commandEncoder);
	}
	
	void NetworkRenderer::drawEdges(Network const& network,
									RenderCommandEncoder* commandEncoder) {
		if (network.edgeCount() == 0) {
			return;
		}
		
		drawEdges_impl(network.edgeCount(),
					   edgeProxyBuffer.get(),
					   commandEncoder);
	}
	
	void NetworkRenderer::drawEdges_impl(std::size_t count,
										 Buffer* edgeDataBuffer,
										 RenderCommandEncoder* commandEncoder) {
		commandEncoder->setDepthStencilState(depthStencilStateOpaque.get());
		
		commandEncoder->setRenderPipelineState(edgePipelineState.get());

		commandEncoder->setVertexBuffer(quadVertexBuffer(),   0, 0);
		commandEncoder->setVertexBuffer(uniformBuffer.get(),      0, 1);
		commandEncoder->setVertexBuffer(edgeDataBuffer,     0, 2);

		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);

		commandEncoder->drawPrimitives(PrimitiveTypeTriangle,
									   /* vertexStart   = */ 0,
									   /* vertexCount   = */ 6,
									   /* instanceCount = */ count);
	}
	
	void NetworkRenderer::drawNodes(Network const& network, RenderCommandEncoder* commandEncoder) {
		if (network.nodeCount() == 0) {
			return;
		}
		commandEncoder->setDepthStencilState(depthStencilStateOpaque.get());

		commandEncoder->setRenderPipelineState(nodePipelineState.get());

		commandEncoder->setVertexBuffer(quadVertexBuffer(),             0, 0);
		commandEncoder->setVertexBuffer(uniformBuffer.get(),            0, 1);
		commandEncoder->setVertexBuffer(nodePositionBuffer.get(),       0, 2);
		commandEncoder->setVertexBuffer(nodeSizeBuffer.get(),           0, 3);
		commandEncoder->setVertexBuffer(nodeCategoryBuffer.get(),       0, 4);
		commandEncoder->setVertexBuffer(nodeFlagsBuffer.get(),          0, 5);
		commandEncoder->setVertexBuffer(nodePinCountBuffer.get(),       0, 6);
		commandEncoder->setVertexBuffer(nodeBuildProgressBuffer.get(),  0, 7);

		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);

		commandEncoder->drawPrimitives(PrimitiveTypeTriangle,
									   /* vertexStart   = */ 0, /* vertexCount   = */ 6,
									   /* instanceCount = */ network.nodeCount());
	}

	void NetworkRenderer::drawNodeShadows(Network const& network, RenderCommandEncoder* commandEncoder) {
		if (network.nodeCount() == 0) {
			return;
		}

		commandEncoder->setDepthStencilState(depthStencilStateTranslucent.get());

		commandEncoder->setRenderPipelineState(nodeShadowPipelineState.get());

		commandEncoder->setVertexBuffer(quadVertexBuffer(),        0, 0);
		commandEncoder->setVertexBuffer(uniformBuffer.get(),       0, 1);
		commandEncoder->setVertexBuffer(nodePositionBuffer.get(),  0, 2);
		commandEncoder->setVertexBuffer(nodeSizeBuffer.get(),      0, 3);

		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);

		commandEncoder->drawPrimitives(PrimitiveTypeTriangle,
									   /* vertexStart   = */ 0, /* vertexCount   = */ 6,
									   /* instanceCount = */ network.nodeCount());
	}

	
	void NetworkRenderer::drawSelection(SelectionRectangle rect,
										RenderCommandEncoder* commandEncoder) {
		fillBuffer(selectionDataBuffer.get(), rect.get());
		
		commandEncoder->setRenderPipelineState(selectionPipelineState.get());

		commandEncoder->setVertexBuffer(quadVertexBuffer(),        0, 0);
		commandEncoder->setVertexBuffer(uniformBuffer.get(),       0, 1);
		commandEncoder->setVertexBuffer(selectionDataBuffer.get(), 0, 2);

		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);

		commandEncoder->drawPrimitives(PrimitiveTypeTriangle,
									   /* vertexStart   = */ 0ul, /* vertexCount   = */ 6);
	}
	
	static float screenPixelRange(float quadScreenSize, float glyphAtlasSize, float dfSize) {
		return quadScreenSize / glyphAtlasSize * dfSize;
	}
	
	static unsigned glyphAtlasIndex(float quadScreenSize, float dfSize) {
		unsigned i = 0;
		for (auto value: { 8, 16, 32, 64, 128, 256 }) {
			if (quadScreenSize < value) {
				return i;
			}
			++i;
		}
		return i - 1;
	}
	
	void NetworkRenderer::drawNames(Network const& network, Font font, float textSize, float scale,
									RenderCommandEncoder* commandEncoder)
	{
		if (network.nodeCount() == 0) { return; }

		commandEncoder->setRenderPipelineState(nodeNamePipelineState.get());

		auto const textScreenSize = textSize * scale;


		auto const textureIndex = glyphAtlasIndex(textScreenSize, 2);

		auto const screenPXRange = screenPixelRange(textScreenSize, glyphAtlasSizes[textureIndex], 2);
		
		LabelRenderData const labelRenderData = {
			.size = textSize,
			.pixelRange = screenPXRange
		};
		fillBuffer(nodeNameRenderDataBuffer.get(), labelRenderData);
		
		static_assert(alignof(NetworkUniforms) >= alignof(LabelRenderData));

		auto const glyphRenderData = textRenderer->glyphRenderData(font, glyphAtlasSizes[textureIndex]);

		commandEncoder->setVertexBuffer(textRenderer->vertexBuffer(),           0, 0);
		auto* const letterBuffer = nodeNameBuffer.get();
		commandEncoder->setVertexBuffer(letterBuffer,                           0, 1);
		commandEncoder->setVertexBuffer(glyphRenderData->glyphDataBuffer.get(), 0, 2);
		commandEncoder->setVertexBuffer(uniformBuffer.get(),                    0, 3);
		commandEncoder->setVertexBuffer(nodeNameRenderDataBuffer.get(),         0, 4);
		commandEncoder->setVertexBuffer(nodePositionBuffer.get(),               0, 5);
		commandEncoder->setVertexBuffer(nodeSizeBuffer.get(),                   0, 6);


		commandEncoder->setFragmentBuffer(glyphRenderData->fontDataBuffer.get(),  0, 0);
		commandEncoder->setFragmentBuffer(uniformBuffer.get(),            0, 1);
		commandEncoder->setFragmentTexture(glyphRenderData->atlas.get(),  0);
		commandEncoder->setFragmentSamplerState(textRenderer->samplerState(), 0);

		commandEncoder->drawPrimitives(PrimitiveTypeTriangle,
									   0ul, 6 * nodeNameMaxRenderSize,
									   network.nodeCount());
	}
	
	void NetworkRenderer::drawNameFrames(Network const& network,
										 RenderCommandEncoder* commandEncoder) {
		WM_DebugBreak("not implemented");
	}
	
	void NetworkRenderer::drawBackground(RenderCommandEncoder* commandEncoder) {
		commandEncoder->setRenderPipelineState(backgroundPipelineState.get());

		commandEncoder->setVertexBuffer(quadVertexBuffer(), 0, 0);

		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);
		commandEncoder->drawPrimitives(PrimitiveTypeTriangle, 0ul, 6);
	}
	
	template <typename Field>
	static void copySingleNodeBuffer(Network const& network, Buffer* buffer) {
		fillBuffer(buffer, network.nodes().data<Field>(), network.nodeCount() * sizeof(Field));
	}
	
	void NetworkRenderer::updateNodeBuffers(Network const& network) {
		// fill node Buffers
		if (nodeBufferSize < network.nodeCount()) {
			createNodeBuffers(network.nodeCount());
			nodeBufferSize = network.nodeCount();
		}
		
		if (network.nodes().empty()) {
			return;
		}
		
		copySingleNodeBuffer<Node::Position>(network, nodePositionBuffer.get());
		copySingleNodeBuffer<Node::Size>(network, nodeSizeBuffer.get());
		copySingleNodeBuffer<NodeCategory>(network, nodeCategoryBuffer.get());
		copySingleNodeBuffer<Node::Flags>(network, nodeFlagsBuffer.get());
		copySingleNodeBuffer<PinCount<>>(network, nodePinCountBuffer.get());
		copySingleNodeBuffer<Node::BuildProgress>(network, nodeBuildProgressBuffer.get());
		
		createNodeNameRenderData(network);
		
		fillBuffer(nodeNameBuffer.get(), nodeNameRenderData.data(),
				   sizeof(NodeNameRenderData) * nodeNameRenderData.size());
	}
	
	static std::string shortenName(std::string name) {
		if (name.size() <= nodeNameMaxRenderSize) {
			  return name;
		  }
		  else {
			  name.resize(nodeNameMaxRenderSize);
			  *(name.end() - 1) = '.';
			  *(name.end() - 2) = '.';
			  *(name.end() - 3) = '.';
			  return name;
		  }
	  }
	
	void NetworkRenderer::createNodeNameRenderData(Network const& network) {
		nodeNameRenderData.resize(network.nodeCount());
		
		for (std::size_t i = 0; i < network.nodeCount(); ++i) {
			std::string const nameShortened = shortenName(network.nodes().get<Node::Name>(i));
			
			auto const letterData = typeSetter->typeset(nameShortened, {
				FontWeight::regular, FontStyle::roman
			}, TextAlignment::center);
			WM_Assert(letterData.letters.size() <= nodeNameMaxRenderSize, "Letter count excess");

			NodeNameRenderData letterDataArray{};

			std::copy(letterData.letters.begin(),
					  letterData.letters.end(),
					  letterDataArray.begin());
			
			nodeNameRenderData[i] = letterDataArray;
		}
	}
	
}
