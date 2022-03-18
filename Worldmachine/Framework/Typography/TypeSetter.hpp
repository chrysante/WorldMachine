#pragma once

#include <string>
#include <utl/vector.hpp>
#include <mtl/mtl.hpp>
#include <utl/hashmap.hpp>
#include <utl/memory.hpp>
#include <optional>
#include <filesystem>
#include "TextRenderData.hpp"

template <>
struct std::hash<worldmachine::Font> {
	std::size_t operator()(worldmachine::Font const& font) const {
		using namespace utl;
		std::size_t seed = 0x5f23ef3b;
		seed = hash_combine(seed, static_cast<std::size_t>(font.weight));
		seed = hash_combine(seed, static_cast<std::size_t>(font.style));
		return seed;
	}
};

namespace worldmachine {
	
	struct TypesetResult {
		utl::vector<LetterData> letters;
		float width;
	};
	
	class TypeSetter: public utl::enable_ref_from_this<TypeSetter> {
		friend class TextRenderer;
	public:
		TypeSetter();
		TypesetResult typeset(std::string_view, Font font, TextAlignment alignment = TextAlignment::left) const;
		
		std::optional<FontMetrics> metrics(Font font, std::size_t size) const {
			auto const itr = _fontData.find({ font, size });
			if (itr == _fontData.end()) {
				return std::nullopt;
			}
			else {
				return itr->second.metrics;
			}
		}
		
	private:
		struct FontData {
			utl::vector<GlyphData> glyphData;
			double size{};
			FontMetrics metrics;
			FontAtlasData atlasData;
		};
		
	private:
		static std::uint32_t glyphIndex(char);
		void parseFontData();
		FontData parseFontData(Font font, std::size_t size);
		static std::string resourceName(Font, std::size_t size);
		
		FontData const* fontData(Font font, std::size_t size) const {
			auto const itr = _fontData.find({ font, size });
			if (itr == _fontData.end()) {
				return nullptr;
			}
			else {
				return &itr->second;
			}
		}
		
	private:
		utl::hashmap<std::pair<Font, std::size_t>, FontData, utl::hash<std::pair<Font, std::size_t>>> _fontData;
		utl::hashmap<Font, utl::vector<float>> _advance;
	};
	
}


