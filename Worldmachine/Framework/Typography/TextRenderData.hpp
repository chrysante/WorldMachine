#pragma once

#include "Core/Base.hpp"
#include "Framework/ShaderMath.hpp"

#if WORLDMACHINE(CPP) && WORLDMACHINE_GLYPH_DATA_IMPL
#include <nlohmann/json.hpp>
#endif // WORLDMACHINE(CPP) && WORLDMACHINE_GLYPH_DATA_IMPL

#if WORLDMACHINE(CPP)

#include "Core/Debug.hpp"
#include <utl/utility.hpp>
#include <string>
#include <utl/concepts.hpp>

#endif // WORLDMACHINE(CPP)

namespace worldmachine {
	
	enum struct TextAlignment: int8_t {
		left,
		center,
		right
	};
	
	enum struct FontWeight: int8_t {
		ultralight,
		thin,
		light,
		regular,
		medium,
		semibold,
		bold,
		heavy,
		black,
		_count
	};
	
	enum struct FontStyle: int8_t {
		roman,
		italic,
		_count
	};
	
	struct Font {
		FontWeight weight = FontWeight::regular;
		FontStyle style = FontStyle::roman;
		
#if WORLDMACHINE(CPP)
		
		static void forEach(utl::invocable<Font> auto&& f) {
			for (std::underlying_type_t<FontStyle> style = 0;
				 style < utl::to_underlying(FontStyle::_count);
				 ++style) {
				for (std::underlying_type_t<FontWeight> weight = 0;
					 weight < utl::to_underlying(FontWeight::_count);
					 ++weight) {
					f(Font{ FontWeight{ weight }, FontStyle{ style } });
				}
			}
		}
		
		bool operator==(Font const&) const = default;
		
#endif
	};
	
	static_assert(sizeof(Font) == 2, "");
	
#if WORLDMACHINE(CPP)
	
	inline std::string toString(FontWeight w) {
		switch (w) {
#define WM_ENUM_STRING_CASE_IMPL(x) case FontWeight::x: return #x
				WM_ENUM_STRING_CASE_IMPL(ultralight);
				WM_ENUM_STRING_CASE_IMPL(thin);
				WM_ENUM_STRING_CASE_IMPL(light);
				WM_ENUM_STRING_CASE_IMPL(regular);
				WM_ENUM_STRING_CASE_IMPL(medium);
				WM_ENUM_STRING_CASE_IMPL(semibold);
				WM_ENUM_STRING_CASE_IMPL(bold);
				WM_ENUM_STRING_CASE_IMPL(heavy);
				WM_ENUM_STRING_CASE_IMPL(black);
#undef WM_ENUM_STRING_CASE_IMPL
			default:
				WM_DebugBreak("invalid value");
		}
	}
	
	inline std::string toString(FontStyle s) {
		switch (s) {
#define WM_ENUM_STRING_CASE_IMPL(x) case FontStyle::x: return #x
				WM_ENUM_STRING_CASE_IMPL(roman);
				WM_ENUM_STRING_CASE_IMPL(italic);
#undef WM_ENUM_STRING_CASE_IMPL
			default:
				WM_DebugBreak("invalid value");
		}
	}
	
#endif
	
	struct BoundingBox {
		float bottom;
		float top;
		float left;
		float right;
	};
	
	struct GlyphData {
		uint32_t unicode;
		float advance;
		BoundingBox quadBounds;
		BoundingBox atlasBounds;
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
	
#if WORLDMACHINE(CPP) && WORLDMACHINE_GLYPH_DATA_IMPL
	
	static void from_json(const nlohmann::json& j, BoundingBox& g) {
		if (j.is_null()) {
			g = {};
		}
		else {
			j.at("bottom").get_to(g.bottom);
			j.at("top"   ).get_to(g.top);
			j.at("left"  ).get_to(g.left);
			j.at("right" ).get_to(g.right);
		}
	}
	static void from_json(const nlohmann::json& j, GlyphData& d) {
		if (j.is_null()) {
			d = {};
		}
		else {
			d.unicode = j["unicode"];
			d.advance = j["advance"];
			if (d.unicode == 32) /* space */ {
				d.quadBounds = {};
				d.atlasBounds = {};
			}
			else {
				d.quadBounds  = j["planeBounds"];
				d.atlasBounds = j["atlasBounds"];
			}
			d.size.x = d.quadBounds.right - d.quadBounds.left;
			d.size.y = d.quadBounds.top   - d.quadBounds.bottom;
		}
	}
	static void from_json(const nlohmann::json& j, FontMetrics& g) {
		if (j.is_null()) {
			g = {};
		}
		else {
			g.emSize             = j["emSize"];
			g.lineHeight         = j["lineHeight"];
			g.ascender           = j["ascender"];
			g.descender          = j["descender"];
			g.underlineY         = j["underlineY"];
			g.underlineThickness = j["underlineThickness"];
		}
	}
	
	static void from_json(const nlohmann::json& j, FontAtlasData& g) {
		if (j.is_null()) {
			g = {};
		}
		else {
			g.distanceRange = j["distanceRange"];
			g.size          = j["size"];
			g.width         = j["width"];
			g.height        = j["height"];
		}
	}
	
#endif // WORLDMACHINE(CPP) && WORLDMACHINE_GLYPH_DATA_IMPL
	
}
