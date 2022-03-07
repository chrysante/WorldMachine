#include "UUID.hpp"

#include <atomic>
#include <ostream>
#include "format.hpp"

#include <random>
#include <array>
#include "bit.hpp"

namespace utl {

	static UUID::value_type _generate() {
		std::mt19937_64 rng(std::random_device{}());
		std::array<utl::uint64_t, 2> value = { rng(), rng() };
		
#if UTL(_128_BIT_ARITHMETIC)
		static_assert(std::is_same_v<typename UUID::value_type, utl::uint128_t>);
		return utl::bit_cast<typename UUID::value_type>(value);
#else
		return value;
#endif
	}
	
	UUID UUID::generate() {
		return UUID(_generate());
	}
	
	std::ostream& operator<<(std::ostream& str, UUID id) {
		return str << utl::format("{:x}", id.value());
	}
	
}
