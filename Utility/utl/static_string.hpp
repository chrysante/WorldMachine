#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <string>
#include <cstddef>
#include <iosfwd>

namespace utl {
	
	template <std::size_t Cap>
	struct basic_static_string {
	public:
		basic_static_string() = default;
		template <std::size_t N>
		requires(N < Cap)
		constexpr basic_static_string(char const (&str)[N]): basic_static_string(std::make_index_sequence<N>{}, str) {}
		
		constexpr basic_static_string(char const* str): basic_static_string(std::make_index_sequence<Cap - 1>{}, str) {}
		
		explicit operator std::string() const { return std::string(_data); }
		
		constexpr operator std::string_view() const { return std::string_view(_data, size()); }
		
		constexpr std::size_t size() const noexcept { return utl::strlen(_data); }
		constexpr static std::size_t max_size() noexcept { return Cap - 1; }
		
		constexpr char const* c_str() const noexcept { return _data; }
		constexpr char const* data() const noexcept { return _data; }
		
		constexpr char* begin() { return _data; }
		constexpr char const* begin() const { return _data; }
		
		constexpr char* end() { return _data + size(); }
		constexpr char const* end() const { return _data + size(); }
		
		bool operator==(basic_static_string const&) const = default;
		
		template <std::size_t Length>
		constexpr basic_static_string<Length + 1> substr(std::size_t begin) const noexcept {
			return basic_static_string<Length + 1>(_data + begin);
		}
		
		template<std::nullptr_t = nullptr>
		inline friend std::ostream& operator<<(std::ostream& str, basic_static_string const& s) {
			return [&](auto& x) -> decltype(auto) {
				return x << s._data;
			}(str);
		}
		
		char _data[Cap];
		
	private:
		template <std::size_t... I>
		constexpr basic_static_string(std::index_sequence<I...>, char const* str): _data{ str[I]..., '\0' } {}
	};

	template <std::size_t N>
	basic_static_string(char const (&str)[N]) -> basic_static_string<N + 1>;
	
	using static_string128 = basic_static_string<128>;
	
	template <std::size_t N, std::size_t M>
	requires (N != M)
	constexpr bool operator==(basic_static_string<N> const&, basic_static_string<M> const&) { return false; }
	
}
