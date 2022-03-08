#pragma once

#pragma GCC system_header

#include "basic_macros.hpp"

#define UTL(x) UTL_PRIVATE_DEFINITION_##x()

#define UTL_PRIVATE_DEFINITION_SYSTEM_HEADER() _Pragma("GCC system_header")

#ifdef UTL_DEBUG_LEVEL
#define UTL_PRIVATE_DEFINITION_DEBUG_LEVEL() UTL_DEBUG_LEVEL
#else
#define UTL_PRIVATE_DEFINITION_DEBUG_LEVEL() 0
#endif

#ifndef UTL_CPP_INTERFACE
#if __cplusplus && !__METAL_VERSION__
#define UTL_PRIVATE_DEFINITION_CPP() 1
#else // __cplusplus
#define UTL_PRIVATE_DEFINITION_CPP() 0
#endif // __cplusplus
#else // UTL_CPP_INTERFACE
#define UTL_PRIVATE_DEFINITION_CPP() UTL_CPP_INTERFACE()
#endif // UTL_CPP_INTERFACE

#if UTL(CPP)
#if __cplusplus >= 202002L
#define UTL_PRIVATE_DEFINITION_CPP_VERSION() 20
#elif __cplusplus == 201703L
#define UTL_PRIVATE_DEFINITION_CPP_VERSION() 17
#elif __cplusplus == 201402L
#define UTL_PRIVATE_DEFINITION_CPP_VERSION() 14
#elif __cplusplus == 201103L
#define UTL_PRIVATE_DEFINITION_CPP_VERSION() 11
#else // __cplusplus <=> ...
#define UTL_PRIVATE_DEFINITION_CPP_VERSION() 0
#endif // __cplusplus <=> ...
#endif // UTL(CPP)

#ifndef UTL_C_INTERFACE
#ifdef __cplusplus
#define UTL_PRIVATE_DEFINITION_C()   0
#else // __cplusplus
#define UTL_PRIVATE_DEFINITION_C()   1
#endif // __cplusplus
#else // UTL_C_INTERFACE
#define UTL_PRIVATE_DEFINITION_C() UTL_C_INTERFACE()
#endif // UTL_C_INTERFACE

#if UTL(CPP)

#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <climits>

#include "__private/optimization_flags.hpp"

#define UTL_INTERNAL_CONCAT_IMPL(a, b) a##b
#define UTL_INTERNAL_CONCAT(a, b) UTL_INTERNAL_CONCAT_IMPL(a, b)
#define UTL_INTERNAL_UNIQUE_NAME(name) UTL_INTERNAL_CONCAT(name##_, __LINE__)

#define UTL_ENABLE_IF_DEFINITION(...) typename std::enable_if<__VA_ARGS__, std::nullptr_t>::type
#define UTL_ENABLE_IF(...) UTL_ENABLE_IF_DEFINITION(__VA_ARGS__) = nullptr

#define UTL_ENABLE_MEMBER_IF(...) std::nullptr_t UTL_INTERNAL_UNIQUE_NAME(_re2_dummy) = nullptr, typename std::enable_if<!static_cast<void*>(UTL_INTERNAL_UNIQUE_NAME(_re2_dummy)) && (__VA_ARGS__), std::nullptr_t>::type = nullptr

#endif // UTL(CPP)

#if UTL(CPP)

#include <utility>

#define UTL_FORWARD(x) ::std::forward<decltype(x)>(x)

#define UTL_PRIVATE_DEFINITION_NODISCARD() [[nodiscard]]
#define UTL_PRIVATE_DEFINITION_MAYBE_UNUSED() [[maybe_unused]]

#define UTL_PRIVATE_DEFINITION_api_export() UTL_ATTRIB(NODEBUG)

#define UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK __attribute__((no_sanitize("integer")))

#endif // UTL(CPP)()





/// MARK: - Attributes
#if __clang__
#define __utl_pure                 __attribute__((const))
#define __utl_nodiscard            [[nodiscard]]
#if MTL_DEBUG_LEVEL > 0
#	define __utl_always_inline
#else // MTL_DEBUG_LEVEL > 0
#	define __utl_always_inline     __attribute__((always_inline))
#endif // MTL_DEBUG_LEVEL > 0

#define __utl_noinline             __attribute__((noinline))
#if MTL_DEBUG_LEVEL > 0
#	define __utl_interface_export
#else // MTL_DEBUG_LEVEL > 0
#	define __utl_interface_export  //__attribute__((nodebug))
#endif // MTL_DEBUG_LEVEL > 0

#endif // __clang__




#if UTL(CPP)
#include <cstdint>
#include <popcntintrin.h>
namespace utl {
	
	struct __private_tag{};
	
	constexpr bool is_constant_evaluated() noexcept {
#ifdef __cpp_lib_is_constant_evaluated
		return std::is_constant_evaluated();
#else
		return false;
#endif
	}
	
	struct no_init_t{} constexpr no_init{};
	
	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value)>
	constexpr T round_up(T x, std::size_t multiple_of) noexcept {
		return x % multiple_of == 0 ? x : x + multiple_of - x % multiple_of;
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value)>
	constexpr std::size_t popcount(T x) noexcept {
		if (is_constant_evaluated()) {
			std::size_t result = 0;
			for (std::size_t i = 0; i < sizeof(T) * CHAR_BIT; ++i) {
				if ((1 << i) & x) {
					++result;
				}
			}
			return result;
		}
		else {
			return __builtin_popcountll(x);
		}
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value)>
	constexpr bool is_power_of_two(T x) noexcept {
		return popcount(x) == 1;
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value)>
	constexpr bool is_aligned(T x, std::size_t alignment) noexcept {
		return x % alignment == 0;
	}
	
	inline bool is_aligned(void* ptr, std::size_t alignment) noexcept {
		return is_aligned(reinterpret_cast<std::uintptr_t>(ptr), alignment);
	}
	
	inline void* offset(void* ptr, std::ptrdiff_t offset) noexcept {
		return static_cast<char*>(ptr) + offset;
	}
	
	inline void const* offset(void const* ptr, std::ptrdiff_t offset) noexcept {
		return static_cast<char const*>(ptr) + offset;
	}
	
	template <typename T>
	bool is_aligned_for(void* ptr) noexcept {
		return is_aligned(ptr, alignof(T));
	}
	
	template <typename T>
	void destroy(T& t) noexcept {
		t.~T();
	}
	
	template <typename T>
	void destroy(T* t) noexcept {
		t->~T();
	}
	
	inline namespace literals {
	
		constexpr std::size_t operator""_size(unsigned long long i) noexcept { return i; }
		
		constexpr std::uint8_t  operator""_u8 (unsigned long long i) noexcept { return static_cast<std::uint8_t>(i); }
		constexpr std::uint16_t operator""_u16(unsigned long long i) noexcept { return static_cast<std::uint16_t>(i); }
		constexpr std::uint32_t operator""_u32(unsigned long long i) noexcept { return static_cast<std::uint32_t>(i); }
		constexpr std::uint64_t operator""_u64(unsigned long long i) noexcept { return static_cast<std::uint64_t>(i); }
		
		constexpr std::int8_t  operator""_i8 (unsigned long long i) noexcept { return static_cast<std::int8_t>(i); }
		constexpr std::int16_t operator""_i16(unsigned long long i) noexcept { return static_cast<std::int16_t>(i); }
		constexpr std::int32_t operator""_i32(unsigned long long i) noexcept { return static_cast<std::int32_t>(i); }
		constexpr std::int64_t operator""_i64(unsigned long long i) noexcept { return static_cast<std::int64_t>(i); }
	
	}
		
	template <typename T>
	struct tag { using type = T; };
	
	template <typename T>
	constexpr typename std::add_const<T>::type& as_const(T& t) noexcept {
		return t;
	}
	template <typename T>
	void as_const(const T&&) = delete;
	template <typename T>
	constexpr T const* as_const(T* t) noexcept {
		return t;
	}
	
	template <typename T>
	constexpr typename std::remove_const<T>::type& as_mutable(T& t) noexcept {
		return const_cast<typename std::remove_const<T>::type&>(t);
	}
	template <typename T>
	void as_mutable(const T&&) = delete;
	template <typename T>
	constexpr T* as_mutable(T const* t) noexcept {
		return const_cast<T*>(t);
	}
	
	template <typename T>
	constexpr T* as_mutable_ptr(T const* a) noexcept {
		return const_cast<T*>(a);
	}
	template <typename T>
	constexpr T* as_mutable_ptr(T* a) noexcept {
		return a;
	}
	
	
#define UTL_CALL_AS_CONST(PTR, ...) ::utl::as_mutable(::utl::as_const(PTR)->__VA_ARGS__)
	
	
	template <typename T, typename... U>
	constexpr decltype(auto) first(T&& t, U&&...) { return std::forward<T>(t); }
	
	template <typename T, typename... U>
	constexpr auto are_equal(T&& t, U&&... u) {
		if constexpr (sizeof...(U) == 0) {
			return true;
		}
		else {
			return ((t == u) && ...);
		}
	}
	
	constexpr std::size_t strlen(char const* str) {
		std::size_t result = 0;
		while (*str != '\0') {
			++str;
			++result;
		}
		return result;
	}
	
	static_assert(strlen("") == 0);
	static_assert(strlen("1") == 1);
	static_assert(strlen("1234567890") == 10);
	static_assert(strlen("abcdefghij") == 10);
	
}


namespace utl {
	
	template <typename...>
	struct type_tuple;

	namespace _private {
		
		template <typename T, std::size_t Size, typename... Args>
		struct make_type_tuple_impl {
			using type = typename make_type_tuple_impl<T, Size - 1, Args..., T>::type;
		};
		
		template <typename T, typename... Args>
		struct make_type_tuple_impl<T, 0, Args...> {
			using type = type_tuple<Args...>;
		};
	}
	
	template <typename T, std::size_t Size>
	using make_type_tuple = typename _private::make_type_tuple_impl<T, Size>::type;
	
	template <std::size_t... I>
	using index_sequence = std::index_sequence<I...>;
	
	template <std::size_t N>
	using make_index_sequence = std::make_index_sequence<N>;
	
}

namespace utl {
	
	using uint8_t   = std::uint8_t;
	using uint16_t  = std::uint16_t;
	using uint32_t  = std::uint32_t;
	using uint64_t  = std::uint64_t;
	using uint128_t = __uint128_t;
	using int8_t    = std::int8_t;
	using int16_t   = std::int16_t;
	using int32_t   = std::int32_t;
	using int64_t   = std::int64_t;
	using int128_t  = __int128_t;
#define UTL_PRIVATE_DEFINITION__128_BIT_ARITHMETIC() 1

}


#if UTL(CPP)
namespace utl {
	template <auto...> struct ctprint;
}

#define UTL_CTPRINT(...) ::utl::ctprint<__VA_ARGS__> UTL_ANONYMOUS_VARIABLE(_UTL_CTPRINT)
#endif // UTL(CPP)



#define utl_guard(...) if (__VA_ARGS__) {}
#if UTL_GUARD_NO_PREFIX
#define guard(...) utl_guard(__VA_ARGS__)
#endif // UTL_GUARD_NO_PREFIX
  
#include "operator_definition_macros.hpp"

#endif // UTL(CPP)



