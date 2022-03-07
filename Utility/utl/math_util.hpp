#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "type_traits.hpp"
#include "debug.hpp"
#include <cmath>

//namespace utl {
//	enum struct interpolation {
//		linear, cubic, quintic, sqrt, inverted_sqrt
//	};
//}

namespace utl::_private {
	
	template <typename T, UTL_ENABLE_IF(std::is_floating_point<T>::value)>
	UTL(NODISCARD) constexpr T signed_sqrt(T x) {
		if (std::signbit(x)) {
			return -std::sqrt(-x);
		}
		else {
			return std::sqrt(x);
		}
	}
	template <typename T, typename U, UTL_ENABLE_IF(std::is_floating_point<T>::value), UTL_ENABLE_IF(std::is_floating_point<U>::value)>
	UTL(NODISCARD) constexpr auto signed_pow(T x, U y) {
		if (std::signbit(x)) {
			return -std::pow(-x, y);
		}
		else {
			return std::pow(x, y);
		}
	}
	
	template <typename T, typename U = T, UTL_ENABLE_IF(std::is_integral_v<T>)>
	UTL(NODISCARD) constexpr T ceil_divide(T a, U b) noexcept {
		return (a / b) + !!(a % b);
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_floating_point_v<T>)>
	UTL(NODISCARD) constexpr T fract(T f) noexcept {
		float i;
		auto const result = std::modf(f, &i);
		return (result < 0) + result;
	}
	
	template <typename T, typename U, UTL_ENABLE_IF(std::is_floating_point_v<T>), UTL_ENABLE_IF(std::is_arithmetic_v<U>)>
	UTL(NODISCARD) constexpr auto mod(T f, U r) noexcept {
		return _private::fract(f / r) * r;
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_floating_point_v<T>)>
	UTL(NODISCARD) constexpr T floor(T const& f) noexcept {
		return std::floor(f);
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_floating_point_v<T>)>
	UTL(NODISCARD) constexpr T ceil(T const& f) noexcept {
		return std::ceil(f);
	}
}

namespace utl {
	
	template <typename T, typename A>
	UTL(NODISCARD) constexpr T mix(T const& t, T const& u, A const& alpha) {
		return t * (1 - alpha) + u * alpha;
	}
	
}
