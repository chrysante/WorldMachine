#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "vector_math.hpp"
#include <cmath>
#include "complex_decl.hpp"

namespace utl::inline math {
	
	template <typename T>
	struct complex: public vec<T, 2, default_vector_options> {
		using base = vec<T, 2, default_vector_options>;
		
	public:
		using value_type = T;
		
	public:
		complex() = default;
		constexpr complex(T x, T y = 0): base{ x, y } {}
		constexpr complex(vec2<T> const& v): base(v) {}
		using base::base;
		
		constexpr T&       real()       noexcept { return (*this)[0]; }
		constexpr T const& real() const noexcept { return (*this)[0]; }
		
		constexpr T&       imag()       noexcept { return (*this)[1]; }
		constexpr T const& imag() const noexcept { return (*this)[1]; }
		
		template <typename U>
		constexpr operator complex<U>() const {
			return {
				static_cast<U>((*this)[0]),
				static_cast<U>((*this)[1])
			};
		}
		
		constexpr complex operator+() const { return *this; }
		constexpr complex operator-() const { return { -real(), -imag() }; }
	};
	
	template <typename T, typename U>
	complex(T, U) -> complex<typename promote<T, U>::type>;
	
	namespace math_literals {
		
		inline complex<float> operator""_i(long double x) noexcept { return { 0, static_cast<float>(x) }; }
		inline complex<int> operator""_i(unsigned long long x) noexcept { return { 0, static_cast<int>(x) }; }
		
	};
	
	template <typename T, typename U>
	constexpr bool operator==(complex<T> const& a, complex<U> const& b) {
		return a.real() == b.real() && a.imag() == b.imag();
	}
	template <typename T, typename U>
	constexpr bool operator!=(complex<T> const& a, complex<U> const& b) {
		return !(a == b);
	}
	template <typename T, typename U, UTL_ENABLE_IF(is_scalar<U>::value)>
	constexpr bool operator==(complex<T> const& a, U b) {
		return a.real() == b && a.imag() == 0;
	}
	template <typename T, typename U, UTL_ENABLE_IF(is_scalar<U>::value)>
	constexpr bool operator!=(complex<T> const& a, U b) {
		return !(a == b);
	}
	template <typename T, typename U, UTL_ENABLE_IF(is_scalar<T>::value)>
	constexpr bool operator==(T a, complex<U> const& b) {
		return a == b.real() && 0 == b.imag();
	}
	template <typename T, typename U, UTL_ENABLE_IF(is_scalar<T>::value)>
	constexpr bool operator!=(T a, complex<U> const& b) {
		return !(a == b);
	}
	
	template <typename T>
	constexpr complex<T> conjugate(complex<T> const& z) {
		return { z.real(), -z.imag() };
	}
	
	template <typename T>
	constexpr complex<T> inverse(complex<T> const& z) {
		if constexpr (std::is_integral<T>::value) {
			utl_ensure(norm_squared(z) == 1, "complex integer is not invertible");
			return conjugate(z);
		}
		else {
			return conjugate(z) / norm_squared(z);
		}
	}
	
	template <typename T = default_float_type>
	typename to_floating_point<T>::type arg(complex<T> const& z) {
		return std::atan2(z.imag(), z.real());
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator+(complex<T> const& a, complex<U> const& b) {
		return map(a, b, plus);
	}
	
	template <typename T, typename U>
	constexpr complex<typename promote<T, U>::type> operator+(T a, complex<U> const& b) {
		return { a + b.real(), b.imag() };
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator+(complex<T> const& a, U b) {
		return { a.real() + b, a.imag() };
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator-(complex<T> const& a, complex<U> const& b) {
		return map(a, b, minus);
	}
	
	template <typename T, typename U>
	constexpr complex<typename promote<T, U>::type> operator-(T a, complex<U> const& b) {
		return { a - b.real(), -b.imag() };
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator-(complex<T> const& a, U b) {
		return { a.real() - b, a.imag() };
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator*(complex<T> const& a, complex<U> const& b) {
		return { a.real() * b.real() - a.imag() * b.imag(), a.real() * b.imag() + a.imag() * b.real() };
	}
	
	template <typename T, typename U>
	constexpr complex<typename promote<T, U>::type> operator*(T a, complex<U> const& b) {
		return map(b, [&](auto x) { return a * x; });
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator*(complex<T> const& a, U b) {
		return map(a, [&](auto x) { return x * b; });
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator/(complex<T> const& a, complex<U> const& b) {
		return a * inverse(b);
	}

	template <typename T, typename U>
	constexpr complex<typename promote<T, U>::type> operator/(T a, complex<U> const& b) {
		return a * inverse(b);
	}
	
	template <typename T, typename U = T>
	constexpr complex<typename promote<T, U>::type> operator/(complex<T> const& a, U b) {
		return map(a, [&](auto x) { return x / b; });
	}
	
	template <typename T>
	auto expi(T z) {
		return complex{ std::cos(z), std::sin(z) };
	}
	
	template <typename T>
	auto exp(complex<T> const& z) {
		return std::exp(z.real()) * expi(z.imag());
	}
	
	template <typename T, typename U>
	complex<typename to_floating_point<T, U>::type> pow(complex<T> const& base, complex<U> const& exp) {
		using ResultType = typename to_floating_point<T, U>::type;
		auto const arg_base = arg(base);
		auto const normsq_base = norm_squared(base);
		auto const trig_arg = exp.real() * arg_base + ResultType(0.5) * exp.imag() * std::log(normsq_base);
		return
			std::pow(normsq_base, exp.real() * ResultType(0.5)) *
			std::exp(-exp.imag() * arg_base) *
			complex{ std::cos(trig_arg), std::sin(trig_arg) };
	}
	
	template <typename T>
	complex<typename to_floating_point<T>::type> pow(complex<T> const& base, long exp) {
		if (exp == 0) {
			return 1;
		}
		else if (exp < 0) {
			return pow(inverse(base), -exp);
		}
		else {
			auto const impl = [](auto const& base, auto const& exp, auto impl) {
				if (exp == 1) {
					return base;
				}
				else if (exp % 2 == 0) {
					return impl(base * base, exp / 2, impl);
				}
				else {
					return base * impl(base * base, exp / 2, impl);
				}
			};
			return impl(base, exp, impl);
		}
	}
	
}

template <typename T, utl::real_number U>
struct utl::promote<utl::complex<T>, U> {
	using type = utl::complex<typename utl::promote<T, U>::type>;
};
template <utl::real_number T, typename U>
struct utl::promote<T, utl::complex<U>> {
	using type = utl::complex<typename utl::promote<T, U>::type>;
};
