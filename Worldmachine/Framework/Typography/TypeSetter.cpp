#define WORLDMACHINE_GLYPH_DATA_IMPL 1
#include "TypeSetter.hpp"

#include "Core/Debug.hpp"
#include "ResourceUtil.hpp"

#include <sstream>
#include <fstream>

#include <nlohmann/json.hpp>
#include <utl/strcat.hpp>
#include <utl/format.hpp>

namespace worldmachine {
	
	struct {
		std::size_t first, last;
	} inline constexpr supportedUnicodeRange = { 0x20, 0x80 };
	
	TypeSetter::TypeSetter() {
		parseFontData();
	}
	
	TypesetResult TypeSetter::typeset(std::string_view string, Font font, TextAlignment alignment) const {
		TypesetResult result;
		result.letters.reserve(string.size());
		float offsetX = 0;
		auto& letters = result.letters;
		auto& advanceList = _advance.at(font);
		
		for (auto c: string) {
			auto const glyphIndex = this->glyphIndex(c);
			auto const& advance = advanceList[glyphIndex];
			
			letters.push_back({
				.glyphIndex = glyphIndex,
				.offset = { offsetX, 0 }
			});
			
			offsetX += advance;
		}
		result.width = offsetX;
		
		switch (alignment) {
			case TextAlignment::center:
				for (auto& l: letters) {
					l.offset.x -= result.width / 2;
				}
				break;
			case TextAlignment::right:
				for (auto& l: letters) {
					l.offset.x -= result.width;
				}
				break;
				
			default:
				break;
		}
		
		return result;
	}
	
	std::uint32_t TypeSetter::glyphIndex(char c) {
		if (supportedUnicodeRange.first <= c && c < supportedUnicodeRange.last) {
			return static_cast<std::uint32_t>(c - supportedUnicodeRange.first);
		}
		else {
			return glyphIndex('?');
		}
	}
	
	void TypeSetter::parseFontData() {
		using namespace utl;
		Font::forEach([this](Font font) {
			for (auto size: { 8, 16, 32, 64, 128, 256 }) {
				_fontData[std::pair{ font, size }] = parseFontData(font, size);
			}
			auto& advance = _advance[font];
			
			advance.reserve(_fontData[{ font, 8 }].glyphData.size());
			for (auto const& data: _fontData[{ font, 8 }].glyphData) {
				advance.push_back(data.advance);
			}
		});
		
	}
	
	TypeSetter::FontData TypeSetter::parseFontData(Font font, std::size_t size) {
		using nlohmann::json;
		
		
		auto fontPath = pathForResource(std::filesystem::path{ "Font" } / this->resourceName(font, size), "json");
		std::fstream file(fontPath, std::ios::in);
		if (!file) {
			std::runtime_error("Failed to open File");
		}
		std::stringstream sstr;
		sstr << file.rdbuf();
		
		auto const atlasDataJSON = std::move(sstr).str();
		
		json atlasData = json::parse(atlasDataJSON);
		
		TypeSetter::FontData result;
		
		result.glyphData.reserve(atlasData.size());
		
		result.size = atlasData["atlas"]["size"];
		
		result.metrics = atlasData["metrics"];
		result.atlasData = atlasData["atlas"];
		
		for (auto& json_glyph: atlasData["glyphs"]) {
			result.glyphData.push_back(json_glyph);
			GlyphData& glyph = result.glyphData.back();
			
			auto& atlasBounds = glyph.atlasBounds;
			atlasBounds.left   /= result.atlasData.width;
			atlasBounds.right  /= result.atlasData.width;
			atlasBounds.bottom /= result.atlasData.height;
			atlasBounds.top    /= result.atlasData.height;
			atlasBounds.bottom = 1 - atlasBounds.bottom;
			atlasBounds.top    = 1 - atlasBounds.top;
			
			// UV
			glyph.uv[0] = { atlasBounds.left, atlasBounds.bottom };
			glyph.uv[1] = { atlasBounds.left, atlasBounds.top };
			glyph.uv[2] = { atlasBounds.right,  atlasBounds.top };

			glyph.uv[3] = { atlasBounds.left, atlasBounds.bottom };
			glyph.uv[4] = { atlasBounds.right,  atlasBounds.top };
			glyph.uv[5] = { atlasBounds.right,  atlasBounds.bottom };
		}
		return result;
	}
	
	std::string TypeSetter::resourceName(Font font, std::size_t size) {
		auto const capitalize = [](auto x) {
			if (!x.empty()) {
				x[0] = std::toupper(x[0]);
			}
			return x;
		};
		return utl::format("SFPro-{}-{}-MTSDF-{}",
						   capitalize(toString(font.weight)),
						   capitalize(toString(font.style)),
						   size);
	}
	
}
