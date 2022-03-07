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
	
	TextRenderer::TextRenderer(TypeSetter const* typeSetter, Device* device) {
		this->typeSetter = typeSetter;
		this->device = device;

		_samplerState = createSamplerState(device);
		_vertexBuffer = createVertexBuffer(device);
	}
	
	TextRenderer::GlyphRenderData TextRenderer::loadGlyphRenderData(Device* device, Font font, std::size_t size) {
		GlyphRenderData glyphRenderData;
		auto const atlasName = TypeSetter::resourceName(font, size);
		glyphRenderData.atlas = loadTextureFromFile(device, std::filesystem::path{ "Font" } / atlasName, "png");
		auto& fontData = typeSetter->_fontData.at({ font, size });
		FontData const metAtl = {
			typeSetter->_fontData.at({ font, size }).metrics,
			typeSetter->_fontData.at({ font, size }).atlasData
		};
		glyphRenderData.fontDataBuffer = device->newBuffer(&metAtl,
														   sizeof(FontData),
														   ResourceStorageModeManaged);
		glyphRenderData.glyphDataBuffer = device->newBuffer(fontData.glyphData.data(),
															fontData.glyphData.size() * sizeof(GlyphData),
															ResourceStorageModeManaged);
		return glyphRenderData;
	}
	
	RenderPipelineState* TextRenderer::createPipelineState(Device* device, Library* library,
																   std::string_view vertexShader,
																   std::string_view fragmentShader) const {
		
		auto desc = makeDefaultPipelineStateDescriptor(library, vertexShader, fragmentShader, true, true);
		
		NS::Error* errors;
		return device->newRenderPipelineState(desc.get(), &errors);
	}
	
	TextRenderer::GlyphRenderData const* TextRenderer::glyphRenderData(Font font, std::size_t size) {
		auto const itr = _glyphRenderData.find({ font, size });
		if (itr == _glyphRenderData.end()) {
			[[maybe_unused]] auto [newItr, success] =_glyphRenderData.insert_or_assign({font, size}, loadGlyphRenderData(device, font, size));
			bool _success = success;
			WM_Assert(_success, "Failed to insert Glyph Render Data");
			return &newItr->second;
		}
		else {
			return &itr->second;
		}
	}
	
}


