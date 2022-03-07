#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "type_traits.hpp"

#include <cstdint>
#include <atomic>
#include <popcntintrin.h>

#if UTL(CPP_VERSION) >= 20 && 0
#include <bit>
#else
#include <cstring>
#endif


namespace utl {
	
	template<typename To, typename From,
			 UTL_ENABLE_IF(sizeof(To) == sizeof(From)),
			 UTL_ENABLE_IF(std::is_trivially_copyable<To>::value),
			 UTL_ENABLE_IF(std::is_trivially_copyable<From>::value)>
	constexpr To bit_cast(From const& from) noexcept {
#if UTL(CPP_VERSION) >= 20 && 0
		return std::bit_cast<To>(from);
#else
		typename std::aligned_storage<sizeof(To), alignof(To)>::type storage;
		std::memcpy(&storage, &from, sizeof(To));
		return reinterpret_cast<To&>(storage);
#endif
	}
	
	
	template<typename To, typename From,
			 UTL_ENABLE_IF(sizeof(To) == sizeof(From))>
	constexpr To unsafe_bit_cast(From const& from) noexcept {
		typename std::aligned_storage<sizeof(To), alignof(To)>::type storage;
		std::memcpy(&storage, &from, sizeof(To));
		return reinterpret_cast<To&>(storage);
	}
	
	template<typename To,
			 UTL_ENABLE_IF(std::is_trivially_copyable<To>::value)>
	constexpr To bit_cast_from_ptr(void const* from) noexcept {
		typename std::aligned_storage<sizeof(To), alignof(To)>::type storage;
		std::memcpy(&storage, from, sizeof(To));
		return reinterpret_cast<To&>(storage);
	}
	
	template <typename T = std::size_t>
	constexpr T bit(std::size_t index) { return T(1) << index; }

	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	constexpr inline std::uint16_t byte_swap(std::uint16_t val) {
		return (val << 8) | (val >> 8);
	}

	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	constexpr inline std::int16_t byte_swap(std::int16_t val) {
		return (val << 8) | ((val >> 8) & 0xFF);
	}

	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	constexpr inline std::uint32_t byte_swap(std::uint32_t val) {
		val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
		return (val << 16) | (val >> 16);
	}
	
	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	constexpr inline std::int32_t byte_swap(std::int32_t val) {
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
		return (val << 16) | ((val >> 16) & 0xFFFF);
	}

	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	constexpr inline std::uint64_t byte_swap(std::uint64_t val) {
		val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
		val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
		return (val << 32) | (val >> 32);
	}

	UTL_INTERNAL_DISABLE_UBSAN_INTEGER_CHECK
	constexpr inline std::int64_t byte_swap(std::int64_t val) {
		val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
		val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
		return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
	}

	template <typename T, typename OP, UTL_ENABLE_IF(std::is_integral<T>::value || std::is_enum<T>::value)>
	T manipulate_bit(std::atomic<T> &a, OP bit_op) {
		T val = a.load();
		while (!a.compare_exchange_weak(val, bit_op(val)));

		return val;
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value || (std::is_enum<T>::value && utl::has_operator_bitwise_or<T, unsigned>::value))>
	T set_bit(std::atomic<T> &a, unsigned n) {
		return manipulate_bit(a, [&](auto val) { return val | (1 << n); });
	}
	
	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value || (std::is_enum<T>::value && utl::has_operator_bitwise_and<T, unsigned>::value))>
	T clear_bit(std::atomic<T> &a, unsigned n) {
		return manipulate_bit(a, [&](auto val) { return val & ~(1 << n); });
	}

	template <typename T, UTL_ENABLE_IF(std::is_integral<T>::value || (std::is_enum<T>::value && utl::has_operator_bitwise_xor<T, unsigned>::value))>
	T toggle_bit(std::atomic<T> &a, unsigned n) {
		return manipulate_bit(a, [&](auto val) { return val ^ (1 << n); });
	}
	
	
	template <typename T/*, UTL_ENABLE_IF(std::is_integral<T>::value || (std::is_enum<T>::value && utl::has_operator_bitwise_or<T>::value))*/>
	T atomic_or(std::atomic<T> &a, T b) {
		return manipulate_bit(a, [&](auto val) { return val | b; });
	}
	
	template <typename T/*, UTL_ENABLE_IF(std::is_integral<T>::value || (std::is_enum<T>::value && utl::has_operator_bitwise_or<T>::value))*/>
	T atomic_and(std::atomic<T> &a, T b) {
		return manipulate_bit(a, [&](auto val) { return val & b; });
	}
	
	template <typename T/*, UTL_ENABLE_IF(std::is_integral<T>::value || (std::is_enum<T>::value && utl::has_operator_bitwise_or<T>::value))*/>
	T atomic_xor(std::atomic<T> &a, T b) {
		return manipulate_bit(a, [&](auto val) { return val ^ b; });
	}
	
	
}

