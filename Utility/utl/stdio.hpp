#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <cstddef>
#include <iosfwd>
#include <array>
#include "strcat.hpp"
#include "static_string.hpp"
#include "type_traits.hpp"
#include "format.hpp"
#include "vector.hpp"
#include "__private/fmt/ostream.h"

namespace utl {
	struct stream_ref_wrapper;
	stream_ref_wrapper with_color(std::ostream&, bool value = true);
	struct stream_ref_wrapper {
		stream_ref_wrapper(std::ostream& stream): stream_ref_wrapper(stream, false) {}
		
		
		friend stream_ref_wrapper with_color(std::ostream& stream, bool value) { return { stream, value }; }
		
		std::ostream* _stream;
		bool _using_colors;
	private:
		stream_ref_wrapper(std::ostream& stream, bool force_colors);
	};
	
	namespace format_codes {
		class format_code;
		struct format_code_array;
		namespace _private {
			std::string    _decay(format_code code,        bool uses_color);
			std::string    _decay(format_code_array array, bool uses_color);
			decltype(auto) _decay(auto&& x,                bool) { return UTL_FORWARD(x); }
		}
	}
}

namespace utl {
	
	stream_ref_wrapper set_global_output_stream(stream_ref_wrapper);
	inline stream_ref_wrapper set_global_output_stream(std::ostream& stream) {
		return set_global_output_stream(stream_ref_wrapper{ stream });
	}
	std::tuple<stream_ref_wrapper, std::unique_lock<std::mutex>> get_global_output_stream();
	
	template <typename... Args>
	void print(std::string_view format_string, Args&&... args);
	
	template <typename... Args>
	void print(stream_ref_wrapper stream,
			   std::string_view format_string,
			   Args&&... args);
	
	
	
	template <typename... Args>
	void print(stream_ref_wrapper stream,
			   std::string_view format_string,
			   Args&&... args)
	{
		fmt::print(*stream._stream, format_string,
				   format_codes::_private::_decay(args, stream._using_colors)...);
	}
	
	
	template <typename... Args>
	void print(std::string_view format_string, Args&&... args) {
		
		auto [stream, lock] = get_global_output_stream();
		auto formatted = utl::format(format_string,
									 format_codes::_private::_decay(args, stream._using_colors)...);
		utl::print(stream, formatted);
	}
	
}

namespace utl::format_codes {
	
	class format_code {
	public:
		constexpr format_code(int value): _value(value) {}
		constexpr format_code(int value, char const* unicode):
			_value(value)
		  , _unicode(unicode)
		{}
		
		friend std::string _private::_decay(format_code c, bool uses_color);

		format_code_array operator|(format_code c) const;
		
		constexpr int value() const { return _value; }
		
	private:
		char _value;
		char const* _unicode = nullptr;
	};
	
	inline std::string _private::_decay(format_code c, bool uses_color) {
		if (!uses_color) {
			if (!c._unicode) {
				return {};
			}
			return utl::strcat(c._unicode);
		}
		if (c.value() < 0) /* reset-sentinel */ {
			return "\033[m";
		}
		return utl::strcat("\033[", c.value(), "m");
	}
	
	struct format_code_array: utl::small_vector<format_code> {
		using utl::small_vector<format_code>::small_vector;
		format_code_array(format_code c): utl::small_vector<format_code>({ c }) {}
	};
	
	
	inline std::string _private::_decay(format_code_array array, bool uses_color) {
		std::stringstream sstr;
		for (auto c: array) {
			sstr << _decay(c, uses_color);
		}
		return sstr.str();
	}

	
	inline format_code_array operator|(format_code_array array, format_code c) {
		array.push_back(c);
		return array;
	}
	
	
	inline format_code_array operator|(format_code_array lhs, format_code_array const& rhs) {
		lhs.reserve(lhs.size() + rhs.size());
		for (auto c: rhs) {
			lhs.push_back(c);
		}
		return lhs;
	}
	
	inline format_code_array format_code::operator|(format_code c) const {
		return { *this, c };
	}
	
	inline constexpr auto reset  = format_code(-1);
	
	
	
	inline constexpr auto roman                  = format_code(0);
	inline constexpr auto bold                   = format_code(1);
	inline constexpr auto italic                 = format_code(3);
	inline constexpr auto underline              = format_code(4);
	inline constexpr auto flashing               = format_code(5);
	inline constexpr auto invert_background      = format_code(7);
	
	inline constexpr auto gray                   = format_code(90);
	inline constexpr auto red                    = format_code(91,  "🟥 ");
	inline constexpr auto green                  = format_code(92,  "🟩 ");
	inline constexpr auto yellow                 = format_code(93,  "🟨 ");
	inline constexpr auto blue                   = format_code(94,  "🟦 ");
	inline constexpr auto pink                   = format_code(95,  "🟪 ");
	inline constexpr auto cyan                   = format_code(96,  "🟦 ");
	inline constexpr auto light_gray             = format_code(97);
	inline constexpr auto white                  = format_code(98);
	
	inline constexpr auto dark_gray              = format_code(30);
	inline constexpr auto dark_red               = format_code(31,  "🟥 ");
	inline constexpr auto dark_green             = format_code(32,  "🟩 ");
	inline constexpr auto dark_yellow            = format_code(33,  "🟨 ");
	inline constexpr auto dark_blue              = format_code(34,  "🟦 ");
	inline constexpr auto dark_pink              = format_code(35,  "🟪 ");
	inline constexpr auto dark_cyan              = format_code(36,  "🟦 ");
	inline constexpr auto dark_white             = format_code(38);
	
	inline constexpr auto background_gray        = format_code(100);
	inline constexpr auto background_red         = format_code(101, "🟥 ");
	inline constexpr auto background_green       = format_code(102, "🟩 ");
	inline constexpr auto background_yellow      = format_code(103, "🟨 ");
	inline constexpr auto background_blue        = format_code(104, "🟦 ");
	inline constexpr auto background_pink        = format_code(105, "🟪 ");
	inline constexpr auto background_cyan        = format_code(106, "🟦 ");
	inline constexpr auto background_light_gray  = format_code(107);
	inline constexpr auto background_light_white = format_code(108);
	
#undef format_code
}

