#pragma once

#include <utl/memory.hpp>
#include <utl/hashmap.hpp>
#include <Metal/Metal.hpp>

#include "Framework/Platform/MacOS/MTLARCPointer.hpp"
#include "Framework/Typography/TypeSetter.hpp"
#include "Framework/Renderer.hpp"

namespace worldmachine {
	
	class TextRenderer: Renderer {
	public:
		TextRenderer(utl::ref<TypeSetter const>);
		
	public:
		MTL::RenderPipelineState* createPipelineState(MTL::Library*,
													  std::string_view vertexShader,
													  std::string_view fragmentShader) const;

	private:
		struct GlyphRenderData {
			MTLARCPointer<MTL::Texture> atlas;
			MTLARCPointer<MTL::Buffer> glyphDataBuffer;
			MTLARCPointer<MTL::Buffer> fontDataBuffer;
		};

		GlyphRenderData loadGlyphRenderData(MTL::Device* device, Font, std::size_t size);
		
	public:
		GlyphRenderData const* glyphRenderData(Font font, std::size_t size);

		MTL::SamplerState const* samplerState() const { return _samplerState.get(); }
		MTL::Buffer const* vertexBuffer() const { return _vertexBuffer.get(); }

	private:
		MTLARCPointer<MTL::SamplerState> _samplerState;
		MTLARCPointer<MTL::Buffer> _vertexBuffer;
		utl::ref<TypeSetter const> typeSetter;
		utl::hashmap<std::pair<Font, std::size_t>, GlyphRenderData, utl::hash<std::pair<Font, std::size_t>>> _glyphRenderData;
	};
	
}
