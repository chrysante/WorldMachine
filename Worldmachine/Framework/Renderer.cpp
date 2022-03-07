#include "Renderer.hpp"

#include "Core/Debug.hpp"

using namespace MTL;
using namespace mtl;

#include "Vertex2D.hpp"

namespace worldmachine {

	MTLARCPointer<MTL::RenderPipelineDescriptor> makeDefaultPipelineStateDescriptor(MTL::Library* lib,
																					std::string_view vertexFunction,
																					std::string_view fragmentFunction,
																					bool usesDepth,
																					bool translucent)
	{
		auto*  desc = RenderPipelineDescriptor::alloc()->init();
		desc->colorAttachments()->object(0)->setPixelFormat(PixelFormatRGBA32Float);
		
		if (usesDepth)
			desc->setDepthAttachmentPixelFormat(PixelFormatDepth32Float);

		desc->setVertexFunction(lib->newFunction(makeNSString(vertexFunction))->autorelease());
		desc->setFragmentFunction(lib->newFunction(makeNSString(fragmentFunction))->autorelease());

		if (translucent) {
			desc->colorAttachments()->object(0)->setBlendingEnabled(true);
			desc->colorAttachments()->object(0)->setRgbBlendOperation(BlendOperationAdd);
			desc->colorAttachments()->object(0)->setAlphaBlendOperation(BlendOperationAdd);

			desc->colorAttachments()->object(0)->setSourceRGBBlendFactor(BlendFactorSourceAlpha);
			desc->colorAttachments()->object(0)->setSourceAlphaBlendFactor(BlendFactorSourceAlpha);
			desc->colorAttachments()->object(0)->setDestinationRGBBlendFactor(BlendFactorOneMinusSourceAlpha);
			desc->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(BlendFactorOneMinusSourceAlpha);
		}
		
		return desc;
	}
	
	Renderer::Renderer() {
		device = MTL::CreateSystemDefaultDevice();
		commandQueue = device->newCommandQueue();
		
		constexpr Vertex2D quadData[6] = {
			{ .position = { 0, 0 }, .uv = { 0, 0 } },
			{ .position = { 1, 0 }, .uv = { 1, 0 } },
			{ .position = { 0, 1 }, .uv = { 0, 1 } },
			
			{ .position = { 0, 1 }, .uv = { 0, 1 } },
			{ .position = { 1, 0 }, .uv = { 1, 0 } },
			{ .position = { 1, 1 }, .uv = { 1, 1 } }
		};
		
		_quadVertexBuffer = device->newBuffer(quadData, 6 * sizeof(Vertex2D), ResourceStorageModeShared);
	}
	
	MTLARCPointer<RenderPipelineDescriptor> Renderer::defaultPipelineDescriptor(MTL::Library* lib,
																				std::string_view vertexShader,
																				std::string_view fragmentShader) const
	{
		return makeDefaultPipelineStateDescriptor(lib, vertexShader, fragmentShader, useDepthBuffer, false);
	}
	
	MTLARCPointer<RenderPipelineDescriptor> Renderer::translucencyPipelineDescriptor(MTL::Library* lib,
																					 std::string_view vertexShader,
																					 std::string_view fragmentShader) const
	{
		return makeDefaultPipelineStateDescriptor(lib, vertexShader, fragmentShader, useDepthBuffer, true);
	}
	
	
	
	MTL::Texture* Renderer::createTexture2D(PixelFormat format, usize2 size, bool mipmapped) const {
		auto const desc = TextureDescriptor::texture2DDescriptor(format, size.x, size.y, mipmapped);
		WM_Assert(size == ulong2(desc->width(), desc->height()));
		return device->newTexture(desc);
	}
	
	void Renderer::generateMipmaps(MTL::Texture* texture) {
		auto* const commandBuffer = commandQueue->commandBuffer();
		auto* const blitCommandEncoder = commandBuffer->blitCommandEncoder();
		blitCommandEncoder->generateMipmaps(texture);
		blitCommandEncoder->endEncoding();
		commandBuffer->commit();
	}
	
	void Renderer::updateRenderTargets(mtl::usize2 size) {
		renderTextureSize = size;
		if (size.x == 0 || size.y == 0) {
			renderTexture = nullptr;
			depthTexture = nullptr;
			return;
		}
		
		int2 const desiredRenderTargetSize = size * MSAA;
		int2 const currentRenderTargetSize = renderTexture ? usize2{
			renderTexture->width(), renderTexture->height()
		} : -1;
		
		if (desiredRenderTargetSize != currentRenderTargetSize) {
			createRenderTargets(desiredRenderTargetSize);
		}
	}
	
	void Renderer::createRenderTargets(usize2 size) {
		MTLARCPointer texDescriptor = TextureDescriptor::alloc()->init();
		texDescriptor->setTextureType(TextureType2D);
		texDescriptor->setWidth(size.x);
		texDescriptor->setHeight(size.y);
		texDescriptor->setPixelFormat(PixelFormatRGBA32Float);
		texDescriptor->setUsage(TextureUsageRenderTarget | TextureUsageShaderRead);
		
		renderTexture = device->newTexture(texDescriptor.get());
		
		if (useDepthBuffer) {
			texDescriptor->setStorageMode(StorageModePrivate);
			texDescriptor->setPixelFormat(PixelFormatDepth32Float);
			depthTexture = device->newTexture(texDescriptor.get());
		}
		
	}
	
	
}

