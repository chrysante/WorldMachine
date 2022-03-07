#include "Core/Debug.hpp"

#include "TextRenderer.hpp"

#include "ResourceUtil.hpp"

#include "Vertex2D.hpp"
#include "Renderer.hpp"
#include <string>
#include <utl/stopwatch.hpp>
#include <utl/array.hpp>


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
	
	auto createVertexBuffer(Device* device) {
		constexpr std::array<float2, 6> quadData = { float2
			{ 0, 0 }, { 0, 1 }, { 1, 1 },
			{ 0, 0 }, { 1, 1 }, { 1, 0 }
		};
		
		return device->newBuffer(quadData.data(),
								 quadData.size() * sizeof(float2),
								 ResourceStorageModeManaged);
	}

}

namespace worldmachine {
	
	TextRenderer::TextRenderer(TypeSetter const& typeSetter, Device* device) {
		
		
		
		Font::forEach([&](Font font) {
			for (auto size: { 8, 16, 32, 64, 128, 256 }) {
				GlyphRenderData& glyphRenderData = _glyphRenderData[{ font, size }];
				auto const atlasName = TypeSetter::resourceName(font, size);
				glyphRenderData.atlas = loadTextureFromFile(device, atlasName);
				auto& fontData = typeSetter._fontData.at({ font, size });
				FontData const metAtl = {
					typeSetter._fontData.at({ font, size }).metrics,
					typeSetter._fontData.at({ font, size }).atlasData
				};
				glyphRenderData.fontDataBuffer = device->newBuffer(&metAtl,
																   sizeof(FontData),
																   ResourceStorageModeManaged);
				glyphRenderData.glyphDataBuffer = device->newBuffer(fontData.glyphData.data(),
																	fontData.glyphData.size() * sizeof(GlyphData),
																	ResourceStorageModeManaged);
			}
		});
		
		_samplerState = createSamplerState(device);
		_vertexBuffer = createVertexBuffer(device);
	}
	
	RenderPipelineState* TextRenderer::createPipelineState(Device* device, Library* library,
																   std::string_view vertexShader,
																   std::string_view fragmentShader) const {
		
		auto desc = makeDefaultPipelineStateDescriptor(library, vertexShader, fragmentShader, true, true);
		
		NS::Error* errors;
		return device->newRenderPipelineState(desc.get(), &errors);
	}
	
}


