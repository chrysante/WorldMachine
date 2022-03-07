#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <iosfwd>
#include <cstddef>
#include <functional>
#include <array>
#include "bit.hpp"
#include "hash.hpp"


namespace utl {
	
	class UUID;
	
	class UUID {
	public:
#if UTL(_128_BIT_ARITHMETIC)
		using value_type = utl::uint128_t;
#else
		using value_type = std::array<std::uint64_t, 2>;
#endif
		
	public:
		UUID() = default;
		static UUID generate();
		value_type value() const noexcept { return _value; }
		
		static UUID construct_from_value(value_type value) {
			return UUID(value);
		}
		
	private:
		UUID(value_type v): _value(v) {}
		
	private:
		value_type _value = 0;
	};
	
	inline bool operator==(UUID a, UUID b) noexcept {
		return a.value() == b.value();
	}
	
	std::ostream& operator<<(std::ostream& str, UUID id);
	
}

template <>
struct std::hash<utl::UUID> {
	std::size_t operator()(utl::UUID id) const {
#if UTL(_128_BIT_ARITHMETIC)
		auto [a, b] = utl::bit_cast<std::array<std::uint64_t, 2>>(id.value());
#else
		auto [a, b] = id.value();
#endif
		auto begin = std::hash<std::uint64_t>{}(a);
		return utl::hash_combine(begin, b);
	}
};
