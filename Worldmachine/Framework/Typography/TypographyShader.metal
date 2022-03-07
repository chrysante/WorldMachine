#include <metal_stdlib>
#include "TypographyShader.h"

using namespace metal;



namespace worldmachine {
	
	TextVertex makeTextVertex(float2 vertPosition,
							  float textSize,
							  device LetterData const* letterBuffer,
							  device GlyphData const* glyphBuffer,
							  uint vertexID) {
		auto const letter = letterBuffer[vertexID / 6];
		auto const glyph = glyphBuffer[letter.glyphIndex];
		
		auto position = vertPosition * glyph.size;
		position.x += letter.offset.x;
		position.y += glyph.quadBounds.bottom;
		
		position *= textSize;
		
		return {
			float4(position, 0, 1),
			glyph.uv[vertexID % 6]
		};
		
	}
	
	static float median(float r, float g, float b) {
		using namespace metal;
		return max(min(r, g), min(max(r, g), b));
	}
	
	static float hardType(metal::float3 msd, float screenPxRange) {
		auto const sd = median(msd.r, msd.g, msd.b);
		auto const screenPxDistance = screenPxRange * (sd - 0.5);
		return metal::clamp(screenPxDistance + 0.5, 0.0, 1.0);
	}
	
	float screenPixelRange(float2 quadScreenSize, float glyphAtlasSize, float dfSize) {
		auto const midQuadSize = (quadScreenSize.x + quadScreenSize.x) / 2;
		return midQuadSize / glyphAtlasSize * dfSize;
	}
	
	float2 textSample(float2 uv,
					  float screenPXRange,
					  texture2d<float> msdf,
					  sampler sampler_) {
		auto const tmsd = msdf.sample(sampler_, uv).rgba;
		return {
			hardType(tmsd.rgb, screenPXRange),
			tmsd.a
		};
	}
	
}
