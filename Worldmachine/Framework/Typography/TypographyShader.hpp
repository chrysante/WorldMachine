#pragma once

#include "TextRenderData.hpp"

namespace worldmachine {
	
	struct TextVertex {
		metal::float4 position [[ position ]];
		metal::float2 uv;
		float pixelRange;
		unsigned textureIndex [[ flat ]];
	};
	
	inline TextVertex makeTextVertex(metal::float2 vertexPosition,
									 float textSize,
									 device LetterData const* letterBuffer,
									 device GlyphData const* glyphBuffer,
									 uint vertexID)
	{
		auto const letter = letterBuffer[vertexID / 6];
		auto const glyph = glyphBuffer[letter.glyphIndex];
		
		auto position = vertexPosition * glyph.size;
		position.x += letter.offset.x;
		position.y += glyph.quadBounds.bottom;
		
		position *= textSize;
		
		return {
			float4(position, 0, 1),
			glyph.uv[vertexID % 6]
		};
	}
	
	inline float median(float r, float g, float b) {
		using namespace metal;
		return max(min(r, g), min(max(r, g), b));
	}
	
	inline float hardType(metal::float3 msd, float screenPxRange) {
		auto const sd = median(msd.r, msd.g, msd.b);
		auto const screenPxDistance = screenPxRange * (sd - 0.5);
		return metal::clamp(screenPxDistance + 0.5, 0.0, 1.0);
	}
	
	inline float screenPixelRange(metal::float2 quadScreenSize,
								  float glyphAtlasSize,
								  float dfSize)
	{
		auto const midQuadSize = (quadScreenSize.x + quadScreenSize.x) / 2;
		return midQuadSize / glyphAtlasSize * dfSize;
	}
	
	metal::float2 textSample(metal::float2 uv,
							 float screenPXRange,
							 metal::texture2d<float> msdf,
							 metal::sampler sampler)
	{
		auto const tmsd = msdf.sample(sampler, uv).rgba;
		return {
			hardType(tmsd.rgb, screenPXRange),
			tmsd.a
		};
	}
	
}
