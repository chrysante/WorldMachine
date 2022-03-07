#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include <utility>
#include <string_view>

namespace utl {
	
	template <typename T>
	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	std::size_t hash_combine(std::size_t seed, T const& v) {
		std::hash<T> const hash;
		return seed ^ hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template <typename T>
	struct hash: std::hash<T> {
		using std::hash<T>::operator();
	};
	
	template <typename T, typename U>
	struct hash<std::pair<T, U>> {
		std::size_t operator()(std::pair<T, U> const& p) const {
			std::size_t seed = 0x5f23ef3b;
			seed = utl::hash_combine(seed, p.first);
			seed = utl::hash_combine(seed, p.second);
			return seed;
		}
	};
	
	constexpr std::size_t hash_string(std::string_view str) {
		std::size_t result = 0xcbf29ce484222325; // FNV offset basis

		for (char c : str) {
			result ^= c;
			result *= 1099511628211; // FNV prime
		}

		return result;
	}
	
}


