#include "HeightmapRenderer3D.hpp"

#include <utl/vector.hpp>

using namespace MTL;
using namespace mtl;
using namespace worldmachine;

#include<iostream>

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
	std::ostream& operator<<(std::ostream& str, HeightmapVertex const& v) {
		return str << int2(v.x, v.y);
	}
}
namespace {
	
	static std::uint32_t indexOfVertexAt(std::size_t i, std::size_t j, usize2 const& vertexResolution) {
		return static_cast<std::uint32_t>(j * vertexResolution.x + i);
	}
	
	static Buffer* createHeightmapVertexBuffer(Device* device, usize2 vertexResolution) {
		utl::vector<HeightmapVertex> vertexData(vertexResolution.x * vertexResolution.y);
		
		for (std::size_t j = 0; j < vertexResolution.y; ++j) {
			for (std::size_t i = 0; i < vertexResolution.x; ++i) {
				vertexData[indexOfVertexAt(i, j, vertexResolution)] = {
					static_cast<std::uint16_t>(i),
					static_cast<std::uint16_t>(j)
				};
			}
		}
		
		Buffer* result = device->newBuffer(vertexData.data(),
										   vertexData.size() * sizeof(HeightmapVertex),
										   ResourceStorageModeShared);
		
		return result;
 	}
	static Buffer* createHeightmapIndexBuffer(Device* device, usize2 quadResolution) {
		utl::vector<std::uint32_t> indexData(quadResolution.x * quadResolution.y * 6);
		std::size_t k = 0;
		usize2 const vertexResolution = quadResolution + 1;
		for (std::size_t j = 0; j < quadResolution.y; ++j) {
			for (std::size_t i = 0; i < quadResolution.x; ++i) {
				// loop over all quads in row major order
				indexData[k + 0] = indexOfVertexAt(i,     j,     vertexResolution);
				indexData[k + 1] = indexOfVertexAt(i + 1, j,     vertexResolution);
				indexData[k + 2] = indexOfVertexAt(i,     j + 1, vertexResolution);
				indexData[k + 3] = indexOfVertexAt(i,     j + 1, vertexResolution);
				indexData[k + 4] = indexOfVertexAt(i + 1, j,     vertexResolution);
				indexData[k + 5] = indexOfVertexAt(i + 1, j + 1, vertexResolution);
				k += 6;
			}
		}
		
		Buffer* result = device->newBuffer(indexData.data(),
										   indexData.size() * sizeof(std::uint32_t),
										   ResourceStorageModeShared);
		
		return result;
	}
}

namespace worldmachine {
	
	HeightmapRenderer3D::HeightmapRenderer3D() {
		useDepthBuffer = true;
		MTLARCPointer library = device->newDefaultLibrary();
		MTLARCPointer psdesc = defaultPipelineDescriptor(library.get(),
														 "heightmapVertexShader",
														 "heightmapFragmentShader");
		NS::Error* errors;
		heightmapPipelineState = device->newRenderPipelineState(psdesc.get(), &errors);
		
		uniformBuffer = device->newBuffer(sizeof(HeightmapRenderUniforms), ResourceStorageModeManaged);
		samplerState = createSamplerState(device.get());
		
		
		MTLARCPointer dsDesc = DepthStencilDescriptor::alloc()->init();
		dsDesc->setDepthWriteEnabled(true);
		dsDesc->setDepthCompareFunction(CompareFunctionLessEqual);
		depthStencilStateOpaque = device->newDepthStencilState(dsDesc.get());
	}
	
	void HeightmapRenderer3D::updateHeightmap(ImageView<float const> heightmap) {
		heightTexture = createTexture2D(PixelFormatR32Float, heightmap.size());
		auto const [width, height] = heightmap.size();
		heightTexture->replaceRegion(MTL::Region(0, 0, width, height), 0, 0,
									 heightmap.data(),
									 sizeof(float) * width,
									 sizeof(float) * width * height);
		
		generateMipmaps(heightTexture.get());
		updateSize(heightmap.size());
	}

	void HeightmapRenderer3D::draw(mtl::double2 targetSize, mtl::double2 scaleFactor,
								   mtl::float4 clearColor,
								   HeightmapRenderUniforms uniforms)
	{
		if (heightTexture == nullptr) {
			WM_Log(error, "We don't have height data. forgot to call updateHeightmap()?");
		}
		
		updateRenderTargets(targetSize * scaleFactor);
		
		uniforms.quadResolution = vertexResolution - 1;
		fillBuffer(uniformBuffer.get(), uniforms);
		
		MTLARCPointer renderPassDesc = RenderPassDescriptor::alloc()->init();
		renderPassDesc->colorAttachments()->object(0)->setTexture(renderTexture.get());
		renderPassDesc->colorAttachments()->object(0)->setLoadAction(LoadActionClear);
		renderPassDesc->colorAttachments()->object(0)->setClearColor(ClearColor(clearColor.r,
																				clearColor.g,
																				clearColor.b,
																				1));
		renderPassDesc->colorAttachments()->object(0)->setStoreAction(StoreActionStore);
		renderPassDesc->depthAttachment()->setTexture(depthTexture.get());
	
		auto* const commandBuffer = commandQueue->commandBuffer();
		auto* const commandEncoder = commandBuffer->renderCommandEncoder(renderPassDesc.get());

		commandEncoder->setRenderPipelineState(heightmapPipelineState.get());
		commandEncoder->setDepthStencilState(depthStencilStateOpaque.get());
		
		commandEncoder->setVertexBuffer(heightmapVertexBuffer.get(), 0, 0);
		commandEncoder->setVertexBuffer(uniformBuffer.get(), 0, 1);
		commandEncoder->setVertexTexture(heightTexture.get(),  0);
		commandEncoder->setVertexSamplerState(samplerState.get(), 0);
		
		commandEncoder->setFragmentBuffer(uniformBuffer.get(), 0, 0);
		commandEncoder->setFragmentTexture(heightTexture.get(),  0);
		commandEncoder->setFragmentSamplerState(samplerState.get(), 0);
		
		commandEncoder->drawIndexedPrimitives(PrimitiveTypeTriangle,
											  /* indexCount = */ (vertexResolution - 1).fold(utl::multiplies) * 6,
											  IndexTypeUInt32,
											  heightmapIndexBuffer.get(), 0);

		commandEncoder->endEncoding();
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
	}
	
	void HeightmapRenderer3D::updateSize(mtl::usize2 size) {
		double const aspectRatio = (double)size.x / size.y;
		usize2 const newVertexResolution = { 512 , 512 * aspectRatio };
		if (vertexResolution != newVertexResolution) {
			vertexResolution = newVertexResolution;
			heightmapVertexBuffer = createHeightmapVertexBuffer(device.get(), vertexResolution);
			heightmapIndexBuffer = createHeightmapIndexBuffer(device.get(), vertexResolution - 1);
		}
	}
	
}
