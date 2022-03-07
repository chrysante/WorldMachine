#pragma once

#ifndef __MTL_QUATERNION_HPP_INCLUDED__
#define __MTL_QUATERNION_HPP_INCLUDED__

#include "__base.hpp"
_MTL_SYSTEM_HEADER_

#include "__common.hpp"
#define __MTL_DECLARE_QUATERNION_TYPEDEFS__
#include "__typedefs.hpp"

#include "vector.hpp"
#include "complex.hpp"

#include "__std_concepts.hpp"
#include <cmath>
#include <iosfwd>

/// MARK: - Synopsis
/*
 
 */

namespace _VMTL {
	
	/// MARK: - struct quaternion
	template <real_scalar T>
	struct quaternion<T>: public vector<T, 4, vector_options{}.packed(false)> {
		static constexpr auto __options = vector_options{}.packed(false);
		using __base = vector<T, 4, __options>;
		using typename __base::value_type;
		
	public:
		using __vector_data_<T, 4, __options>::real;
		using __vector_data_<T, 4, __options>::imag;
		using __vector_data_<T, 4, __options>::vector;
		
		using __imag_t = _VMTL::vector<T, 3, __options>;
		
	public:
		/// Value Constructors
		quaternion() = default;
		constexpr quaternion(T real): __base(real, 0, 0, 0) {}
		template <vector_options P>
		constexpr quaternion(T const& real,  _VMTL::vector<T, 3, P> const& imag): __base(real, imag.__at(0), imag.__at(1), imag.__at(2)) {}
		constexpr quaternion(T const& a, T const& b, T const& c, T const& d): __base(a, b, c, d) {}
		constexpr quaternion(complex<T> const& z): quaternion(z.__at(0), z.__at(1), 0, 0) {}
		constexpr quaternion(_VMTL::vector<T, 4, __options> const& v): __base(v) {}
		
		/// Conversion Constructor
		template <_VMTL::convertible_to<T> U>
		constexpr quaternion(quaternion<U> const& rhs): __base(rhs) {}
		
		quaternion& operator=(quaternion const&)& = default;
		
		/// MARK: - Arithmetic Assignment
		constexpr quaternion& operator+=(quaternion const& rhs)& {
			*this = *this + rhs;
			return *this;
		}
		constexpr quaternion& operator+=(T const& rhs)& {
			*this = *this + rhs;
			return *this;
		}
		constexpr quaternion& operator-=(quaternion const& rhs)& {
			*this = *this - rhs;
			return *this;
		}
		constexpr quaternion& operator-=(T const& rhs)& {
			*this = *this - rhs;
			return *this;
		}
		constexpr quaternion& operator*=(quaternion const& rhs)& {
			*this = *this * rhs;
			return *this;
		}
		constexpr quaternion& operator*=(T const& rhs)& {
			*this = *this * rhs;
			return *this;
		}
		constexpr quaternion& operator/=(quaternion const& rhs)& {
			*this = *this / rhs;
			return *this;
		}
		constexpr quaternion& operator/=(T const& rhs)& {
			*this = *this / rhs;
			return *this;
		}
	};
	
	/// MARK: - Misc
	template <real_scalar T>
	quaternion(T) -> quaternion<T>;
	template <real_scalar T, real_scalar U, real_scalar V, real_scalar W>
	quaternion(T, U, V, W) -> quaternion<__mtl_promote(T, U, V, W)>;
	template <real_scalar T, real_scalar U, vector_options O = vector_options{}>
	quaternion(T, _VMTL::vector<U, 3, O>) -> quaternion<__mtl_promote(T, U)>;
	
	template <typename T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr T real(quaternion<T> const& z) { return z.__at(0); }
	template <typename T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, 3, quaternion<T>::__options> imag(quaternion<T> const& z) {
		return {
			z.__at(1), z.__at(2), z.__at(3)
		};
	}
	
	template <typename T>
	__mtl_mathfunction __mtl_always_inline
	constexpr vector<T, 4, quaternion<T>::__options>& __as_vector(quaternion<T>& z) {
		return static_cast<vector<T, 4, quaternion<T>::__options>&>(z);
	}
	template <typename T>
	__mtl_mathfunction __mtl_always_inline
	constexpr vector<T, 4, quaternion<T>::__options> const& __as_vector(quaternion<T> const& z) {
		return static_cast<vector<T, 4, quaternion<T>::__options> const&>(z);
	}
	
	// real, imag:
	
	
	/// MARK: - Literals
	inline namespace literals {
		inline namespace quaternion_literals {
			/*
			 _i* literals are defined in complex.hpp and can be used in conjunction with these
			 ‘using namespace mtl::quaternion_literals;' also pulls in the _i* complex literals
			 */
			inline constexpr quaternion<double>             operator"" _j   (long double x)        { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<float>              operator"" _jf  (long double x)        { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<long double>        operator"" _jld (long double x)        { return { 0, 0, x, 0 }; }
			
			inline constexpr quaternion<int>                operator"" _j   (unsigned long long x) { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<long>               operator"" _jl  (unsigned long long x) { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<long long>          operator"" _jll (unsigned long long x) { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<unsigned int>       operator"" _ju  (unsigned long long x) { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<unsigned long>      operator"" _jul (unsigned long long x) { return { 0, 0, x, 0 }; }
			inline constexpr quaternion<unsigned long long> operator"" _jull(unsigned long long x) { return { 0, 0, x, 0 }; }
			
			inline constexpr quaternion<double>             operator"" _k   (long double x)        { return { 0, 0, 0, x }; }
			inline constexpr quaternion<float>              operator"" _kf  (long double x)        { return { 0, 0, 0, x }; }
			inline constexpr quaternion<long double>        operator"" _kld (long double x)        { return { 0, 0, 0, x }; }
			
			inline constexpr quaternion<int>                operator"" _k   (unsigned long long x) { return { 0, 0, 0, x }; }
			inline constexpr quaternion<long>               operator"" _kl  (unsigned long long x) { return { 0, 0, 0, x }; }
			inline constexpr quaternion<long long>          operator"" _kll (unsigned long long x) { return { 0, 0, 0, x }; }
			inline constexpr quaternion<unsigned int>       operator"" _ku  (unsigned long long x) { return { 0, 0, 0, x }; }
			inline constexpr quaternion<unsigned long>      operator"" _kul (unsigned long long x) { return { 0, 0, 0, x }; }
			inline constexpr quaternion<unsigned long long> operator"" _kull(unsigned long long x) { return { 0, 0, 0, x }; }
		}
	}
	
	/// MARK: - Quaternion Arithmetic
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<T> operator+(quaternion<T> const& z) {
		return z;
	}
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<T> operator-(quaternion<T> const& z) {
		return -__as_vector(z);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator+(quaternion<T> const& a, quaternion<U> const& b) {
		return __as_vector(a) + __as_vector(b);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator+(quaternion<T> const& a, complex<U> const& b) {
		return { real(a) + real(b), imag(b) + a.__at(1), a.__at(2), a.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator+(quaternion<T> const& a, U const& b) {
		return { real(a) + b, a.__at(1), a.__at(2), a.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator+(complex<T> const& a, quaternion<U> const& b) {
		return { real(a) + real(b), imag(a) + b.__at(1), b.__at(2), b.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator+(T const& a, quaternion<U> const& b) {
		return { a + real(b), b.__at(1), b.__at(2), b.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator-(quaternion<T> const& a, quaternion<U> const& b) {
		return __as_vector(a) - __as_vector(b);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator-(quaternion<T> const& a, complex<U> const& b) {
		return { real(a) - real(b), a.__at(1) - imag(b), a.__at(2), a.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator-(quaternion<T> const& a, U const& b) {
		return { real(a) - b, a.__at(1), a.__at(2), a.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator-(complex<T> const& a, quaternion<U> const& b) {
		return { real(a) - real(b), imag(a) - b.__at(1), -b.__at(2), -b.__at(3) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator-(T const& a, quaternion<U> const& b) {
		return { a - real(b), -imag(b) };
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator*(quaternion<T> const& a, quaternion<U> const& b) {
		return {
			a.__at(0) * b.__at(0) - a.__at(1) * b.__at(1) - a.__at(2) * b.__at(2) - a.__at(3) * b.__at(3),  // 1
			a.__at(0) * b.__at(1) + a.__at(1) * b.__at(0) + a.__at(2) * b.__at(3) - a.__at(3) * b.__at(2),  // i
			a.__at(0) * b.__at(2) - a.__at(1) * b.__at(3) + a.__at(2) * b.__at(0) + a.__at(3) * b.__at(1),  // j
			a.__at(0) * b.__at(3) + a.__at(1) * b.__at(2) - a.__at(2) * b.__at(1) + a.__at(3) * b.__at(0)   // k
		};
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator*(quaternion<T> const& a, complex<U> const& b) {
		return {
			 a.__at(0) * b.__at(0) - a.__at(1) * b.__at(1),  // 1
			 a.__at(0) * b.__at(1) + a.__at(1) * b.__at(0),  // i
			 a.__at(2) * b.__at(0) + a.__at(3) * b.__at(1),  // j
			-a.__at(2) * b.__at(1) + a.__at(3) * b.__at(0)   // k
		};
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator*(complex<T> const& a, quaternion<U> const& b) {
		return {
			a.__at(0) * b.__at(0) - a.__at(1) * b.__at(1),  // 1
			a.__at(0) * b.__at(1) + a.__at(1) * b.__at(0),  // i
			a.__at(0) * b.__at(2) - a.__at(1) * b.__at(3),  // j
			a.__at(0) * b.__at(3) + a.__at(1) * b.__at(2)   // k
		};
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator*(quaternion<T> const& a, U const& b) {
		return __as_vector(a) * b;
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator*(T const& a, quaternion<U> const& b) {
		return a * __as_vector(b);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator/(quaternion<T> const& a, quaternion<U> const& b) {
		return a * conj(b) / norm_squared(b);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator/(complex<T> const& a, quaternion<U> const& b) {
		return a * conj(b) / norm_squared(b);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator/(quaternion<T> const& a, complex<U> const& b) {
		return a * conj(b) / norm_squared(b);
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator/(quaternion<T> const& a, U const& b) {
		return __as_vector(a) / b;
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_promote(T, U)> operator/(T const& a, quaternion<U> const& b) {
		return a * conj(b) / norm_squared(b);
	}
	
	/// MARK: - operator==
	template <typename T, typename U>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(quaternion<T> const& a, quaternion<U> const& b) {
		return __as_vector(a) == __as_vector(b);
	}
	
	template <typename T, typename U>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(quaternion<T> const& a, complex<U> const& b) {
		return real(a) == real(b) && a.__at(1) == imag(b) && a.__at(2) == 0 && a.__at(3) == 0;
	}
	
	template <typename T, real_scalar U>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(quaternion<T> const& a, U const& b) {
		return real(a) == b && a.__at(1) == 0 && a.__at(2) == 0 && a.__at(3) == 0;
	}
	
	template <typename T, typename U>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(complex<T> const& a, quaternion<U> const& b) {
		return real(a) == real(b) && imag(a) == b.__at(1) && 0 == b.__at(2) && 0 == b.__at(3);
	}
	
	template <real_scalar T, typename U>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(T const& a, quaternion<U> const& b) {
		return a == real(b) && 0 == b.__at(1) && 0 == b.__at(2) && 0 == b.__at(3);
	}

	
	/// MARK: - Quaternion Math Functions
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<T> conj(quaternion<T> const& z) {
		return { z.__at(0), -z.__at(1), -z.__at(2), -z.__at(3) };
	}
	
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<T> inverse(quaternion<T> const& z) {
		return conj(z) / length_squared(z);
	}
	
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<T> normalize(quaternion<T> const& z) {
		return normalize(__as_vector(z));
	}
	
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr __mtl_floatify(T) abs(quaternion<T> const& z) {
		return norm(z);
	}
	
	/// MARK: Power Functions
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	quaternion<__mtl_floatify(T)> exp(quaternion<T> const& z) {
		auto const r_exp = std::exp(real(z));
		if (z.vector == 0) {
			return r_exp;
		}
		auto const _vnorm = norm(z.vector);
		return r_exp * (std::cos(_vnorm) + quaternion{ 0, z.imag } * (std::sin(_vnorm) / _vnorm));
	}
	
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	quaternion<__mtl_floatify(T)> log(quaternion<T> const& z) {
		__mtl_expect(z != 0);
		auto const _norm = _VMTL::norm(z);
		if (z.vector == 0) {
			return { std::log(_norm), 0, 0, 0 };
		}
		auto const _vnorm = norm(z.vector);
		return { std::log(_norm), z.vector * std::acos(z.real / _norm) / _vnorm };
	}
	
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	quaternion<__mtl_floatify(T)> log10(quaternion<T> const& z) {
		return _VMTL::log(z) / std::log(T(10));
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_floatify(__mtl_promote(T, U))> pow(quaternion<T> const& x, quaternion<U> const& y) {
		return _VMTL::exp(y * _VMTL::log(x));
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_floatify(__mtl_promote(T, U))> pow(complex<T> const& x, quaternion<U> const& y) {
		return _VMTL::exp(y * _VMTL::log(x));
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_floatify(__mtl_promote(T, U))> pow(T const& x, quaternion<U> const& y) {
		return _VMTL::exp(y * std::log(x));
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_floatify(__mtl_promote(T, U))> pow(quaternion<T> const& x, complex<U> const& y) {
		return _VMTL::exp(y * _VMTL::log(x));
	}
	
	template <real_scalar T, real_scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr quaternion<__mtl_floatify(__mtl_promote(T, U))> pow(quaternion<T> const& x, U const& y) {
		return _VMTL::exp(y * _VMTL::log(x));
	}
	
	template <real_scalar T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	quaternion<__mtl_floatify(T)> sqrt(quaternion<T> const& z) {
		__mtl_expect(real(z) >= 0 || imag(z) != 0);
		auto const r = _VMTL::abs(z);
		return std::sqrt(r) * (z + r) / _VMTL::abs(z + r);
	}
	
	/// MARK: Hyperbolic Functions
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> sinh(quaternion<T> const& z) {
		using F = __mtl_floatify(T);
		auto const v_norm = _VMTL::norm(z.vector);
		__mtl_safe_math_if (std::isinf(real(z)) && !std::isinf(v_norm))
			return complex<F>(real(z), F(NAN));
		__mtl_safe_math_if (real(z) == 0 && !std::isinf(v_norm))
			return complex<F>(real(z), F(NAN));
		__mtl_safe_math_if (v_norm == 0 && !std::isinf(real(z)))
			return z;
		return {
			std::sinh(real(z)) * std::cos(v_norm),
			std::cosh(real(z)) * std::sin(v_norm) / v_norm * z.vector
		};
	}
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> cosh(quaternion<T> const& z) {
		using F = __mtl_floatify(T);
		auto const v_norm = _VMTL::norm(z.vector);
		__mtl_safe_math_if (std::isinf(real(z)) && !std::isinf(v_norm))
			return { std::abs(real(z)), F(NAN), F(NAN), F(NAN) };
		__mtl_safe_math_if (real(z) == 0 && !std::isinf(v_norm))
			return { F(NAN), 0, 0, 0 };
		__mtl_safe_math_if (real(z) == 0 && v_norm == 0)
			return { F(1), 0, 0, 0 };
		__mtl_safe_math_if (v_norm == 0 && !std::isinf(real(z)))
			return { std::abs(real(z)), 0, 0, 0 };
		return {
			std::cosh(real(z)) * std::cos(v_norm),
			std::sinh(real(z)) * std::sin(v_norm) / v_norm * z.vector
		};
	}
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> tanh(quaternion<T> const& z) {
		return _VMTL::sinh(z) / _VMTL::cosh(z);
	}
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> asinh(quaternion<T> const& z);
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> acosh(quaternion<T> const& z);
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> atanh(quaternion<T> const& z);
	
	/// MARK: Trigonometric Functions
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> sin(quaternion<T> const& z) {
		auto const v_norm = norm(z.vector);
		return {
			std::sin(real(z)) * std::cosh(v_norm),
			std::cos(real(z)) * std::sinh(v_norm) / v_norm * z.vector
		};
	}
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> cos(quaternion<T> const& z) {
		auto const v_norm = norm(z.vector);
		return {
			 std::cos(real(z)) * std::cosh(v_norm),
			-std::sin(real(z)) * std::sinh(v_norm) / v_norm * z.vector
		};
	}
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> tan(quaternion<T> const& z) {
		return sin(z) / cos(z);
	}
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> asin(quaternion<T> const& z);
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> acos(quaternion<T> const& z);
	
	template <real_scalar T>
	quaternion<__mtl_floatify(T)> atan(quaternion<T> const& z);
	
}

/// MARK: - Decomposition
namespace _VMTL {

	template <std::size_t I, typename T>
	auto const& get(quaternion<T> const& v) {
		if constexpr (I == 0) {
			return v.__at(0);
		}
		else {
			static_assert(I == 1);
			return v.imag;
		}
		
	}
	template <std::size_t I, typename T>
	auto& get(quaternion<T>& v) {
		if constexpr (I == 0) {
			return v.__at(0);
		}
		else {
			static_assert(I == 1);
			return v.imag;
		}
	}
	template <std::size_t I, typename T>
	auto&& get(quaternion<T>&& v) {
		return std::move(get(v));
	}
	template <std::size_t I, typename T>
	auto&& get(quaternion<T> const&& v) {
		return std::move(get(v));
	}
	
}

template <typename T>
struct std::tuple_size<_VMTL::quaternion<T>>: std::integral_constant<std::size_t, 2> {};

template <typename T>
struct std::tuple_element<0, _VMTL::quaternion<T>> {
	using type = T;
};
template <typename T>
struct std::tuple_element<1, _VMTL::quaternion<T>> {
	using type = typename _VMTL::quaternion<T>::__imag_t;
};


#endif // __MTL_QUATERNION_HPP_INCLUDED__
