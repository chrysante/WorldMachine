#include "ImageRenderer2D.hpp"

#include <utl/array.hpp>

#include "Framework/Vertex2D.hpp"

using namespace MTL;
using namespace mtl;
using namespace worldmachine;

namespace {
	
	auto createSamplerState(Device* device) {
		MTLARCPointer desc = SamplerDescriptor::alloc()->init();
		desc->setMinFilter(SamplerMinMagFilterLinear);
		desc->setMagFilter(SamplerMinMagFilterLinear);
		desc->setMipFilter(SamplerMipFilterLinear);
		desc->setMaxAnisotropy(1);
		desc->setSAddressMode(SamplerAddressModeClampToEdge);
		desc->setTAddressMode(SamplerAddressModeClampToEdge);
		desc->setRAddressMode(SamplerAddressModeClampToEdge);
		desc->setNormalizedCoordinates(true);
		
		return device->newSamplerState(desc.get());
	}
	
}


namespace worldmachine {
	
	ImageRenderer2D::ImageRenderer2D() {
		MTLARCPointer library = device->newDefaultLibrary();
		MTLARCPointer psdesc = defaultPipelineDescriptor(library.get(),
														 "image2DVertexShader",
														 "image2DFragmentShader");
		
		NS::Error* errors;
		quadPipelineState = device->newRenderPipelineState(psdesc.get(), &errors);
		uniformBuffer = device->newBuffer(sizeof(int), ResourceStorageModeManaged);
		samplerState = createSamplerState(device.get());
	}

	void ImageRenderer2D::draw(Image const& img)
	{
		if (img.empty()) { return; }
	
		updateRenderTargets(img.size());
		
		auto const pixelFormat = [&]{
			switch (img.dataType()) {
				case DataType::float1:
					return PixelFormatR32Float;
				case DataType::float2:
					return PixelFormatRG32Float;
				
				default:
					return PixelFormatR32Float;
			}
		}();
		
		imageTexture = createTexture2D(pixelFormat, img.size());
		imageTexture->replaceRegion(MTL::Region(0, 0, img.size().x, img.size().y), 0, 0, img.data(), (int)img.dataTypeSize() * img.size().x, (int)img.dataTypeSize() * img.size().fold(utl::multiplies));
		
		generateMipmaps(imageTexture.get());
		
		MTLARCPointer renderPassDesc = RenderPassDescriptor::alloc()->init();
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderTexture.get());
		renderPassDesc->colorAttachments()->object(0)->setLoadAction(LoadActionClear);
		renderPassDesc->colorAttachments()->object(0)->setClearColor(ClearColor(0.2, 0.2, 0.2, 1));
		renderPassDesc->colorAttachments()->object(0)->setStoreAction(StoreActionStore);
	
		auto* const commandBuffer = commandQueue->commandBuffer();
		auto* const commandEncoder = commandBuffer->renderCommandEncoder(renderPassDesc.get());

		commandEncoder->setRenderPipelineState(quadPipelineState.get());

		commandEncoder->setVertexBuffer(quadVertexBuffer(), 0, 0);

		fillBuffer(uniformBuffer.get(), (int)img.dataType());
		
		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);
		commandEncoder->setFragmentTexture(imageTexture.get(),  0);
		commandEncoder->setFragmentSamplerState(samplerState.get(), 0);

		commandEncoder->drawPrimitives(PrimitiveTypeTriangle, 0ul, 6);

		commandEncoder->endEncoding();
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
	}
	
	void ImageRenderer2D::clear() {
		updateRenderTargets(0);
	}
	
}
