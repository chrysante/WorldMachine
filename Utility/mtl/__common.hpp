#pragma once

#ifndef __MTL_COMMON_HPP_INCLUDED__
#define __MTL_COMMON_HPP_INCLUDED__

#include "__base.hpp"
_MTL_SYSTEM_HEADER_

#include <cmath>
#include <utility>
#include "__std_concepts.hpp"

#define __MTL_DECLARE_STDINT_TYPEDEFS__
#include "__typedefs.hpp"

namespace _VMTL {
	
	/// Just so it exists
	inline namespace literals {}
	
	/// MARK: - struct constants
	template <typename T = double>
	requires std::is_floating_point_v<T>
	struct constants {
		static inline constexpr T pi    = 3.1415926535897932384626433832795028841971693993751058209749445923;
		static inline constexpr T e     = 2.7182818284590452353602874713526624977572470936999595749669676277;
		static inline constexpr T sqrt2 = 1.4142135623730950488016887242096980785696718753769480731766797379;
	};
	
	/// MARK: - struct vector_options
	struct vector_options {
		constexpr bool packed() const { return __packed; }
		constexpr vector_options packed(bool b) const {
			auto result = *this;
			result.__packed = b;
			return result;
		}
		
		bool __packed = MTL_DEFAULT_PACKED;
		
		friend constexpr vector_options combine(_VMTL::same_as<vector_options> auto const&... o) {
			return { .__packed = (o.packed() && ...) };
		}
	};
	
	/// MARK: - Forward Declarations
	template <typename T>
	struct complex;
	template <typename T>
	struct quaternion;
	
	template <typename, std::size_t, vector_options = vector_options{}>
	struct vector;
	template <typename, std::size_t, std::size_t, vector_options = vector_options{}>
	struct matrix;
	
	/// MARK: - Type Traits
	template <typename T>
	struct is_real_scalar: std::is_arithmetic<T>{};
	
	template <typename T>
	struct is_complex: std::false_type{};
	template <typename T>
	struct is_complex<complex<T>>: std::true_type{};
	template <typename T>
	struct is_complex<complex<T> const>: std::true_type{};
	template <typename T>
	struct is_complex<complex<T> volatile>: std::true_type{};
	template <typename T>
	struct is_complex<complex<T> const volatile>: std::true_type{};
	template <typename T>
	struct is_quaternion: std::false_type{};
	template <typename T>
	struct is_quaternion<quaternion<T>>: std::true_type{};
	template <typename T>
	struct is_quaternion<quaternion<T> const>: std::true_type{};
	template <typename T>
	struct is_quaternion<quaternion<T> volatile>: std::true_type{};
	template <typename T>
	struct is_quaternion<quaternion<T> const volatile>: std::true_type{};

	template <typename T>
	struct is_vector: std::false_type{};
	template <typename T, std::size_t N, vector_options O>
	struct is_vector<vector<T, N, O>>: std::true_type{};
	template <typename T, std::size_t N, vector_options O>
	struct is_vector<vector<T, N, O> const>: std::true_type{};
	template <typename T, std::size_t N, vector_options O>
	struct is_vector<vector<T, N, O> volatile>: std::true_type{};
	template <typename T, std::size_t N, vector_options O>
	struct is_vector<vector<T, N, O> const volatile>: std::true_type{};
	
	template <typename T>
	struct is_scalar: std::disjunction<is_real_scalar<T>, is_complex<T>, is_quaternion<T>> {};
	
	template <typename>
	struct get_vector_underlying_type;
	
	template <typename T>
	struct get_vector_underlying_type { using type = T; };
	
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_underlying_type<vector<T, N, O>> { using type = T; };
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_underlying_type<vector<T, N, O> const> { using type = T const; };
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_underlying_type<vector<T, N, O> volatile> { using type = T volatile; };
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_underlying_type<vector<T, N, O> const volatile> { using type = T const volatile; };
	
	template <typename>
	struct get_vector_size;
	
	template <typename T>
	struct get_vector_size: std::integral_constant<std::size_t, 1> {};
	
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_size<vector<T, N, O>>: std::integral_constant<std::size_t, N> {};
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_size<vector<T, N, O> const>: std::integral_constant<std::size_t, N> {};
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_size<vector<T, N, O> volatile>: std::integral_constant<std::size_t, N> {};
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_size<vector<T, N, O> const volatile>: std::integral_constant<std::size_t, N> {};
	
	template <typename>
	struct get_vector_options;
	
	template <typename T>
	struct get_vector_options { static constexpr vector_options value = {}; };
	
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_options<vector<T, N, O>> { static constexpr vector_options value = O; };
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_options<vector<T, N, O> const> { static constexpr vector_options value = O; };
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_options<vector<T, N, O> volatile> { static constexpr vector_options value = O; };
	template <typename T, std::size_t N, vector_options O>
	struct get_vector_options<vector<T, N, O> const volatile> { static constexpr vector_options value = O; };
	
	/// MARK: - Concepts
	template <typename T>
	concept real_scalar = is_real_scalar<T>::value;
	template <typename T>
	concept scalar      = is_scalar<T>::value;
	
	template <typename T, typename U, typename ... V>
	concept __any_of = (_VMTL::same_as<T, U> || ... || _VMTL::same_as<T, V>);
	
	/// MARK: Floatify
#define __mtl_floatify(__type) typename _MTL_floatify<__type>::type
	
	template <typename>
	struct _MTL_floatify;
	
	template <typename T>
	requires std::is_arithmetic_v<T>
	struct _MTL_floatify<T>           { using type = double; };
	template <>
	struct _MTL_floatify<float>       { using type = float; };
	template <>
	struct _MTL_floatify<long double> { using type = long double; };

	/// MARK: Stripped Decltype
	#define __mtl_decltype_stripped(__VA_ARGS__) std::decay_t<decltype(__VA_ARGS__)>
	
	/// MARK: Promotion
#define __mtl_promote(...) ::_VMTL::promote_t<__VA_ARGS__>
	
	template <typename... T>
	struct promote;
	
	template <typename... T>
	using promote_t = typename promote<T...>::type;
	
	template <typename T>
	auto __do_promote()    -> T;
	template <typename T, typename U>
	auto __do_promote()    -> decltype(bool{} ? std::declval<T>() : std::declval<U>());
	template <typename T, typename U, typename V, typename... Rest>
	auto __do_promote()    -> decltype(__do_promote<std::remove_reference_t<decltype(__do_promote<T, U>())>, V, Rest...>());
	
	template <typename T, typename U>
	auto __can_promote(int) -> decltype(bool{} ? std::declval<T>() : std::declval<U>(), std::true_type{});
	template <typename, typename>
	auto __can_promote(...) -> decltype(std::false_type{});
	
	template <typename T, typename U>
	requires (decltype(__can_promote<T, U>(0))::value)
	struct promote<T, U> {
		using type = std::remove_reference_t<decltype(__do_promote<T, U>())>;
	};
	
	/// If this template is undefined, then T, U, V have no common type
	template <typename T, typename U, typename... V>
	struct promote<T, U, V...>: promote<typename promote<T, U>::type, V...> {};
	
	/// Promote Complex Numbers
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<_VMTL::complex<T>, _VMTL::complex<U>> {
		using type = _VMTL::complex<typename _VMTL::promote<T, U>::type>;
	};
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<_VMTL::complex<T>, U> {
		using type = _VMTL::complex<typename _VMTL::promote<T, U>::type>;
	};
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<T, _VMTL::complex<U>> {
		using type = _VMTL::complex<typename _VMTL::promote<T, U>::type>;
	};
	
	/// Promote Quaternions
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<_VMTL::quaternion<T>, _VMTL::quaternion<U>> {
		using type = _VMTL::quaternion<typename _VMTL::promote<T, U>::type>;
	};
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<_VMTL::quaternion<T>, _VMTL::complex<U>> {
		using type = _VMTL::quaternion<typename _VMTL::promote<T, U>::type>;
	};
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<_VMTL::quaternion<T>, U> {
		using type = _VMTL::quaternion<typename _VMTL::promote<T, U>::type>;
	};
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<_VMTL::complex<T>, _VMTL::quaternion<U>> {
		using type = _VMTL::quaternion<typename _VMTL::promote<T, U>::type>;
	};
	template <_VMTL::real_scalar T, _VMTL::real_scalar U>
	struct promote<T, _VMTL::quaternion<U>> {
		using type = _VMTL::quaternion<typename _VMTL::promote<T, U>::type>;
	};
	
	/// Promote Vectors
	template <typename T, typename U, std::size_t Size, vector_options O>
	struct promote<vector<T, Size, O>, U> {
		using type = vector<typename promote<T, U>::type, Size, O>;
	};
	template <typename T, typename U, std::size_t Size, vector_options O>
	struct promote<T, vector<U, Size, O>> {
		using type = vector<typename promote<T, U>::type, Size, O>;
	};
	template <typename T, typename U, std::size_t Size,
			  vector_options O, vector_options P>
	struct promote<vector<T, Size, O>, vector<U, Size, P>> {
		using type = vector<typename promote<T, U>::type, Size, combine(O, P)>;
	};
	template <typename T, typename U, std::size_t S1, std::size_t S2,
			  vector_options O, vector_options P>
	struct promote<vector<T, S1, O>, vector<U, S2, P>>; /// Can't promote these
	 
	/// Promote Matrices
	template <typename T, typename U, std::size_t Rows, std::size_t Columns, vector_options O>
	struct promote<matrix<T, Rows, Columns, O>, U> {
		using type = matrix<typename promote<T, U>::type, Rows, Columns, O>;
	};
	template <typename T, typename U, std::size_t Rows, std::size_t Columns, vector_options O>
	struct promote<T, matrix<U, Rows, Columns, O>> {
		using type = matrix<typename promote<T, U>::type, Rows, Columns, O>;
	};
	template <typename T, typename U, std::size_t Rows, std::size_t Columns,
			  vector_options O, vector_options P>
	struct promote<matrix<T, Rows, Columns, O>, matrix<U, Rows, Columns, P>> {
		using type = matrix<typename promote<T, U>::type, Rows, Columns, combine(O, P)>;
	};
	template <typename T, typename U,
			  std::size_t Rows1, std::size_t Columns1,
			  std::size_t Rows2, std::size_t Columns2,
			  vector_options O, vector_options P>
	struct promote<matrix<T, Rows1, Columns2, O>, matrix<U, Rows2, Columns2, P>>; /// Can't promote two matrices of different dimensions
	template <typename T, typename U,
			  std::size_t Size, std::size_t Rows, std::size_t Columns,
			  vector_options O, vector_options P>
	struct promote<matrix<T, Rows, Columns, O>, vector<U, Size, P>>;              /// Can't promote matrices with vectors
	template <typename T, typename U,
			  std::size_t Size, std::size_t Rows, std::size_t Columns,
			  vector_options O, vector_options P>
	struct promote<vector<T, Size, P>, matrix<U, Rows, Columns, O>>;              /// Can't promote vectors with matrices
	
	/// MARK: - enum struct handedness
	enum struct handedness {
		left_handed, right_handed
	};
	inline constexpr handedness left_handed = handedness::left_handed;
	inline constexpr handedness right_handed = handedness::right_handed;
	inline constexpr handedness default_handedness = handedness::left_handed;
	
	/// MARK: Basic Algorithms
	template <typename T>
	constexpr T const& max(T const& a, T const& b) {
		return (a < b) ? b : a;
	}

	template <typename T>
	constexpr T& max(T& a, T& b) {
		return const_cast<T&>(max(const_cast<T const&>(a),
								  const_cast<T const&>(b)));
	}

	template <typename T>
	constexpr T const& max(T const& a, _VMTL::same_as<T> auto const& b, _VMTL::same_as<T> auto const&... c) {
		return max(max(a, b), c...);
	}

	template <typename T>
	constexpr T& max(T& a, _VMTL::same_as<T> auto& b, _VMTL::same_as<T> auto&... c) {
		return const_cast<T&>(max(const_cast<T const&>(a),
								  const_cast<T const&>(b),
								  const_cast<T const&>(c)...));
	}
	
	template <typename T>
	constexpr T const& min(T const& a, T const& b) {
		return (b < a) ? b : a;
	}

	template <typename T>
	constexpr T& min(T& a, T& b) {
		return const_cast<T&>(min(const_cast<T const&>(a),
								  const_cast<T const&>(b)));
	}

	template <typename T>
	constexpr T const& min(T const& a, _VMTL::same_as<T> auto const& b, _VMTL::same_as<T> auto const&... c) {
		return min(min(a, b), c...);
	}

	template <typename T>
	constexpr T& min(T& a, _VMTL::same_as<T> auto& b, _VMTL::same_as<T> auto&... c) {
		return const_cast<T&>(min(const_cast<T const&>(a),
								  const_cast<T const&>(b),
								  const_cast<T const&>(c)...));
	}
	
	/// MARK: - __hash_combine
	template <typename T>
	std::size_t __hash_combine(std::size_t seed, T const& v) {
		std::hash<T> const hash;
		return seed ^ hash(v) + 0x9e37'79b9'43e3'f411 + (seed << 6) + (seed >> 2);
	}
	
	constexpr std::size_t __hash_seed = 0x5f23'ef3b'34b5'e321;
	
	/// MARK: - Helpers
	// forward
#define __mtl_forward(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
	
	// with_index_sequence
#define __MTL_PRIV_WIS_FT(I, S) <std::size_t... I>
#define __MTL_PRIV_WIS_FP(I, S) (::std::index_sequence<I...>)
#define __MTL_PRIV_WIS_FI(I, S) (::std::make_index_sequence<S>{})
#define __mtl_with_index_sequence(Index, ...) \
[&] __MTL_PRIV_WIS_FT Index __MTL_PRIV_WIS_FP Index __VA_ARGS__ __MTL_PRIV_WIS_FI Index
	
	template <typename> inline constexpr bool __mtl_tfalse = false;
	
	template <typename T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool __is_unit(T const& x) {
		if constexpr (std::is_floating_point_v<T>) {
			return x != 0;
		}
		else if constexpr (std::is_integral_v<T>) {
			return x == 1 || x == -1;
		}
		else {
			static_assert(__mtl_tfalse<T>);
		}
	}
	
	template <std::size_t... I>
	using __index_sequence = std::index_sequence<I...>;
	
	template <std::size_t N>
	using __make_index_sequence = std::make_index_sequence<N>;
	
	template <typename... T>
	struct __type_sequence {};
	
	template <typename T, std::size_t N, typename... R>
	struct __make_type_sequence_impl {
		using type = typename __make_type_sequence_impl<T, N - 1, R..., T>::type;
	};
	
	template <typename T, typename... R>
	struct __make_type_sequence_impl<T, 0, R...> {
		using type = __type_sequence<R...>;
	};
	
	template <typename T, std::size_t N>
	using __make_type_sequence = typename __make_type_sequence_impl<T, N>::type;

	
	/// MARK: - class approx
	template <typename>
	class approx;
	
	template <real_scalar T>
	approx(T) -> approx<T>;
	template <typename T, std::size_t Size, vector_options O>
	approx(vector<T, Size, O>) -> approx<vector<T, Size, O>>;
	
	template <typename T>
	inline constexpr T __mtl_float_threshold = 0;
	template <>
	inline constexpr float       __mtl_float_threshold<float>       = 0.000000000000001;
	template <>
	inline constexpr double      __mtl_float_threshold<double>      = 0.000000000000001;
	template <>
	inline constexpr long double __mtl_float_threshold<long double> = 0.000000000000001;
	
	
	
	template <typename T>
	requires std::is_floating_point_v<T>
	bool __nearly_equal(T a, T b, T epsilon = __mtl_float_threshold<T>) {
		T const absA = std::abs(a);
		T const absB = std::abs(b);
		T const diff = std::abs(a - b);

		if (a == b) { // shortcut, handles infinities
			return true;
		}
		else if (a == 0 || b == 0 || (absA + absB < std::numeric_limits<T>::min())) {
			// a or b is zero or both are extremely close to zero
			// relative error is less meaningful here
			return diff < (epsilon * std::numeric_limits<T>::min());
		}
		else { // use relative error
			return diff / _VMTL::min((absA + absB), std::numeric_limits<T>::max()) < epsilon;
		}
	}
	
	template <typename T>
	requires std::is_floating_point_v<T>
	class approx<T> {
	public:
		approx(T z): _value(z) {}
		
		bool __comp_eq(T rhs) const {
			return __nearly_equal(_value, rhs);
		}
		
		friend std::ostream& operator<<(std::ostream& str, approx<T> const& a) {
			return str << a._value;
		}
		
	private:
		T _value;
	};
	
	template <typename T, std::size_t Size, vector_options O>
	requires std::is_floating_point_v<T>
	class approx<vector<T, Size, O>> {
	public:
		approx(vector<T, Size, O> const& z): _value(z) {}
		
		bool __comp_eq(vector<T, Size, O> const& rhs) const {
			return map(_value, rhs, [](auto&& a, auto&& b) {
				return __nearly_equal(a, b);
			}).fold([](auto a, auto b) {
				return a && b;
			});
		}
		
		friend std::ostream& operator<<(std::ostream& str, approx<vector<T, Size, O>> const& a) {
			return str << a._value;
		}
		
	private:
		vector<T, Size, O> _value;
	};
	
	template <typename T, typename U>
	bool operator==(approx<T> const& r, U const& l) {
		return r.__comp_eq(l);
	}
	template <typename T, typename U>
	bool operator==(U const& r, approx<T> const& l) {
		return l.__comp_eq(r);
	}
	
	/// MARK: - Functions on Builtins
	__mtl_pure __mtl_always_inline
	inline constexpr std::uint32_t __byte_swap(std::uint32_t val) {
		val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
		return (val << 16) | (val >> 16);
	}
	
	__mtl_pure __mtl_always_inline
	inline constexpr float __fract(float f) {
		float i;
		float result = std::modf(f, &i);
		return (result < 0.0f) + result;
	}
	__mtl_pure __mtl_always_inline
	inline constexpr double __fract(double f) {
		double i;
		double result = std::modf(f, &i);
		return (result < 0.0) + result;
	}
	__mtl_pure __mtl_always_inline
	inline constexpr long double __fract(long double f) {
		long double i;
		long double result = std::modf(f, &i);
		return (result < 0.0l) + result;
	}
	
	template <typename T, typename U = T>
	requires std::is_integral_v<T> && std::is_integral_v<U>
	__mtl_pure __mtl_always_inline
	constexpr __mtl_promote(T, U) __ceil_divide(T a, U b) {
		return (a / b) + !!(a % b);
	}
	
	
#define MTL_FOR_EACH_BUILTIN_TYPE(F) \
	F(signed   char);                \
	F(unsigned char);                \
	F(signed   short);               \
	F(unsigned short);               \
	F(signed   int);                 \
	F(unsigned int);                 \
	F(signed   long);                \
	F(unsigned long);                \
	F(signed   long long);           \
	F(unsigned long long);           \
	F(float);                        \
	F(double);                       \
	F(long double)
	
#define MTL_FOR_EACH_BUILTIN_ARITHMETIC_TYPE(F) \
	F(signed   int);                            \
	F(unsigned int);                            \
	F(signed   long);                           \
	F(unsigned long);                           \
	F(signed   long long);                      \
	F(unsigned long long);                      \
	F(float);                                   \
	F(double);                                  \
	F(long double)
	
#define MTL_LENGTH_SQUARED(T) \
	__mtl_mathfunction __mtl_pure __mtl_always_inline      \
	inline constexpr T norm_squared(T x) { return x * x; } \
	__mtl_mathfunction __mtl_pure __mtl_always_inline \
	inline constexpr T length_squared(T x) { return norm_squared(x); }
	MTL_FOR_EACH_BUILTIN_TYPE(MTL_LENGTH_SQUARED);
#undef MTL_LENGTH_SQUARED
	
#define MTL_SQR(T)                                 \
	__mtl_mathfunction __mtl_pure __mtl_always_inline  \
	constexpr inline T __sqr(T x) { return x * x; }
	MTL_FOR_EACH_BUILTIN_TYPE(MTL_SQR);
#undef MTL_SQR
	
#define MTL_IPOW(T)                                  \
	__mtl_mathfunction __mtl_pure                    \
	constexpr inline T __ipow(T x, int i) {          \
		__mtl_expect(i >= 0);                        \
		if (i == 1)     return x;                    \
		if (i == 0)     return 1;                    \
		if (i % 2 == 0) return __ipow(x * x, i / 2); \
		return x * __ipow(x * x, i / 2);             \
	}
	MTL_FOR_EACH_BUILTIN_ARITHMETIC_TYPE(MTL_IPOW);
#undef MTL_IPOW
	
#undef MTL_FOR_EACH_BUILTIN_TYPE
	
	// MARK: - Hypot
	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure
	inline T fast_hypot(T a, _VMTL::same_as<T> auto... b) {
		return std::sqrt(((a * a) + ... + (b * b)));
	}

	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure
	inline T __safe_hypot(T a, _VMTL::same_as<T> auto b) {
		return std::hypot(a, b);
	}
	
	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure
	inline T __safe_hypot(T a, _VMTL::same_as<T> auto... b) {
		/// Make all arguments positive ...
		  a = std::abs(a);
		((b = std::abs(b)), ...);
		/// ... put the biggest argument into a ...
		std::swap(a, _VMTL::max(a, b...));
		/// ... and do the math:
		/// b / a <= 1 for all b, so the square of b / a is also <= 1,
		/// so the argument to std::sqrt is <= count of arguments, so no  overflow can occur.
		/// Overflow can only occur if the actual result overflows.
		return a * std::sqrt((1.0 + ... + __sqr(b / a)));
	}
	
	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure __mtl_always_inline __mtl_interface_export
	inline T __hypot(T a) { return std::abs(a); }
	
	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure
	inline T __hypot(T a, _VMTL::same_as<T> auto... b) {
		T sum_of_squares = ((a * a) + ... + (b * b));
		/// Only use __safe_hypot if necessary. This is about 5 times faster on non-overflowing inputs and only ~10% slower on always-overflowing inputs
		__mtl_safe_math_if (isinf(sum_of_squares)) {
			return __safe_hypot(a, b...);
		}
		return std::sqrt(sum_of_squares);
	}

	/// Interface
	template <scalar T, scalar... U>
	__mtl_mathfunction __mtl_pure __mtl_always_inline __mtl_interface_export
	inline auto hypot(T a, U... b) {
		using F = __mtl_floatify(__mtl_promote(T, U...));
		return __hypot((F)a, (F)b...);
	}
	
	// MARK: - pHypot
	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure
	inline T __safe_phypot(T p, T a, _VMTL::same_as<T> auto... b) {
		  a = std::abs(a);
		((b = std::abs(b)), ...);
		std::swap(a, _VMTL::max(a, b...));
		return a * std::pow((1.0 + ... + std::pow(b / a, p)), T(1) / p);
	}
	
	template <scalar T>
	__mtl_mathfunction __mtl_pure __mtl_always_inline __mtl_interface_export
	inline T __phypot(T p, T a) { return std::abs(a); }
	
	template <_VMTL::__any_of<float, double, long double> T>
	__mtl_mathfunction __mtl_pure
	inline T __phypot(T p, T a, _VMTL::same_as<T> auto... b) {
		T sum_of_powers = ((std::pow(std::abs(a), p) + ... + (std::pow(std::abs(b), p))));
		__mtl_safe_math_if (isinf(sum_of_powers)) {
			return __safe_phypot(p, a, b...);
		}
		return std::pow(sum_of_powers, T(1) / p);
	}

	/// Interface
	template <scalar T, scalar... U>
	__mtl_mathfunction __mtl_pure __mtl_always_inline __mtl_interface_export
	inline auto phypot(scalar auto p, T a, U... b) {
		using F = __mtl_floatify(__mtl_promote(T, U...));
		return __phypot((F)p, (F)a, (F)b...);
	}
	

	// MARK: - Basic Function Objects
#define _MTL_FUNCOBJ_DEF_(NAME, PARAMS, EXPR)  \
	struct NAME##_t {                          \
		constexpr auto operator() PARAMS const \
		requires requires { EXPR; }            \
		{ return EXPR; }                       \
	} inline constexpr NAME{}
	
	struct __plus_t {
		constexpr auto operator()(auto const& a) const
		requires requires { +a; }
		{ return +a; }
		constexpr auto operator()(auto const& a, auto const& b) const
		requires requires { a + b; }
		{ return a + b; }
	} inline constexpr __plus{};
	
	struct __minus_t {
		constexpr auto operator()(auto const& a) const
		requires requires { -a; }
		{ return -a; }
		constexpr auto operator()(auto const& a, auto const& b) const
		requires requires { a - b; }
		{ return a - b; }
	} inline constexpr __minus{};
	
	_MTL_FUNCOBJ_DEF_(__multiplies,  (auto const& a, auto const& b), a * b);
	_MTL_FUNCOBJ_DEF_(__divides,     (auto const& a, auto const& b), a / b);
	_MTL_FUNCOBJ_DEF_(__modulo,      (auto const& a, auto const& b), a % b);
	_MTL_FUNCOBJ_DEF_(__equals,      (auto const& a, auto const& b), a == b);
	_MTL_FUNCOBJ_DEF_(__logical_and, (auto const& a, auto const& b), a && b);
	_MTL_FUNCOBJ_DEF_(__logical_or,  (auto const& a, auto const& b), a || b);
	
	struct __abs_t {
		constexpr auto operator()(auto const& a) const {
			using std::abs;
			return abs(a);
		}
	} inline constexpr __abs{};
	
	struct __sqrt_t {
		constexpr auto operator()(auto const& a) const {
			using std::sqrt;
			return sqrt(a);
		}
	} inline constexpr __sqrt{};
	
}


#endif // __MTL_COMMON_HPP_INCLUDED__
