#pragma once

#include "Core/Base.hpp"
#include "Core/Debug.hpp"

#ifdef WORLDMACHINE_CPP

#include <utl/utility.hpp>
#include <iosfwd>
#include <array>

#endif

namespace worldmachine {
	
	enum struct TextAlignment: char {
		left   = 0,
		center = 1,
		right  = 2
	};
	
	enum struct FontWeight: char {
		ultralight = 0,
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
	
	enum struct FontStyle: char {
		roman = 0,
		italic,
		_count
	};
	
	struct Font {
		FontWeight weight = FontWeight::regular;
		FontStyle style = FontStyle::roman;
		
#ifdef WORLDMACHINE_CPP
		static void forEach(utl::invocable<Font> auto&&);
		bool operator==(Font const&) const = default;
#endif
	};
	
#ifdef WORLDMACHINE_CPP
	
	static_assert(sizeof(Font) == 2, "");
	
	inline void Font::forEach(utl::invocable<Font> auto&& f) {
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
	
	inline std::ostream& operator<<(std::ostream& str, FontWeight w) {
		WM_Expect(utl::to_underlying(w) < utl::to_underlying(FontWeight::_count));
		return str << std::array{
			"ultralight",
			"thin",
			"light",
			"regular",
			"medium",
			"semibold",
			"bold",
			"heavy",
			"black"
		}[utl::to_underlying(w)];
	}
	
	inline std::ostream& operator<<(std::ostream& str, FontStyle s) {
		WM_Expect(utl::to_underlying(s) < utl::to_underlying(FontStyle::_count));
		return str << std::array{
			"roman",
			"italic"
		}[utl::to_underlying(s)];
	}
	
#endif
	
}
