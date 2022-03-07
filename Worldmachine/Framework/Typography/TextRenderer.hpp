#pragma once

#include <utl/memory.hpp>
#include <utl/hashmap.hpp>
#include <Metal/Metal.hpp>

#include "Framework/MTLARCPointer.hpp"
#include "Framework/Typography/TypeSetter.hpp"

namespace worldmachine {
	
	class TextRenderer {
	public:
		TextRenderer(TypeSetter const&, MTL::Device* device);
		
	public:
		MTL::RenderPipelineState* createPipelineState(MTL::Device*, MTL::Library*,
													  std::string_view vertexShader,
													  std::string_view fragmentShader) const;

	private:
		struct GlyphRenderData {
			MTLARCPointer<MTL::Texture> atlas;
			MTLARCPointer<MTL::Buffer> glyphDataBuffer;
			MTLARCPointer<MTL::Buffer> fontDataBuffer;
		};

	public:
		GlyphRenderData const* glyphRenderData(Font font, std::size_t size) const {
			auto const itr = _glyphRenderData.find({ font, size });
			if (itr == _glyphRenderData.end()) {
				return nullptr;
			}
			else {
				return &itr->second;
			}
		}

		MTL::SamplerState const* samplerState() const { return _samplerState.get(); }
		MTL::Buffer const* vertexBuffer() const { return _vertexBuffer.get(); }

	private:
		utl::hashmap<std::pair<Font, std::size_t>, GlyphRenderData, utl::hash<std::pair<Font, std::size_t>>> _glyphRenderData;
		MTLARCPointer<MTL::SamplerState> _samplerState;
		MTLARCPointer<MTL::Buffer> _vertexBuffer;
	};
	
}
