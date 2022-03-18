#pragma once

#include "Core/Base.hpp"
#include "Framework/ShaderMath.hpp"

#include "Font.hpp"

namespace worldmachine {
	
	struct GlyphBoundingBox {
		float bottom;
		float top;
		float left;
		float right;
	};
	
	struct GlyphData {
		uint32_t unicode;
		float advance;
		GlyphBoundingBox quadBounds;
		GlyphBoundingBox atlasBounds;
		metal::float2 size;
		metal::float2 uv[6];
	};
	
	struct LetterData {
		uint32_t glyphIndex;
		metal::float2 offset;
	};
	
	
	struct alignas(4) FontMetrics {
		float emSize;
		float lineHeight;
		float ascender;
		float descender;
		float underlineY;
		float underlineThickness;
	};
	
	struct alignas(4) FontAtlasData {
		float distanceRange;
		float size;
		unsigned width;
		unsigned height;
	};
	
	struct FontData {
		FontMetrics metrics;
		FontAtlasData atlas[4];
	};
	
}
