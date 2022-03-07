#pragma once


#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "debug.hpp"
#include "concepts.hpp"
#include "type_traits.hpp"

namespace utl {
	
	template <typename X, typename...T>
	concept __any_of = std::conjunction_v<std::is_same<X, T>...>;
	
	template <typename Enum>
	constexpr std::underlying_type_t<Enum> to_underlying(Enum t) {
		return static_cast<std::underlying_type_t<Enum>>(t);
	}
	
	template <integral To, integral From>
	To narrow_cast(From f) { return static_cast<To>(f); }
	
//	template <real_unsigned_integral To, real_unsigned_integral From>
//	constexpr To narrow_cast(From f) noexcept(!UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS) {
//		if constexpr (std::numeric_limits<To>::digits >= std::numeric_limits<From>::digits) {
//			return f;
//		}
//		else {
//			utl_assert(f <= (From)std::numeric_limits<To>::max(), "bad narrowing cast - overflow");
//			return (To)f;
//		}
//	}
//
//	template <real_signed_integral To, real_signed_integral From>
//	constexpr To narrow_cast(From f) noexcept(!UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS) {
//		if constexpr (std::numeric_limits<To>::digits >= std::numeric_limits<From>::digits) {
//			return f;
//		}
//		else {
//			utl_assert(f <= (From)std::numeric_limits<To>::max(), "bad narrowing cast - overflow");
//			utl_assert(f >= (From)std::numeric_limits<To>::min(), "bad narrowing cast - overflow");
//			return (To)f;
//		}
//	}
	
	template <typename T>
	void uninitialized_relocate(T* from, T* to) {
		if constexpr (utl::is_trivially_relocatable<T>::value) {
			std::memcpy(to, from, sizeof(T));
		}
		else {
			::new ((void*)to) T(std::move(*from));
			std::destroy_at(from);
		}
	}
	
	template <typename InputIter, typename OutputIter>
	void uninitialized_relocate(InputIter inBegin, InputIter inEnd, OutputIter outBegin) {
		using T = std::remove_cvref_t<decltype(*inBegin)>;
		using U = std::remove_reference_t<decltype(*outBegin)>;
		static_assert(std::is_same_v<T, U>);
		constexpr bool triv_reloc = utl::is_trivially_relocatable<T>::value;
		constexpr bool input_contigous  = std::is_pointer_v<InputIter> || std::is_same_v<std::contiguous_iterator_tag, typename std::iterator_traits<InputIter>::iterator_category>;
		constexpr bool output_contigous = std::is_pointer_v<OutputIter> || std::is_same_v<std::contiguous_iterator_tag, typename std::iterator_traits<OutputIter>::iterator_category>;
		if constexpr (triv_reloc && input_contigous && output_contigous) {
			static_assert(std::is_pointer_v<InputIter>,  "Other Cases are disabled to make ubsan happy");
			static_assert(std::is_pointer_v<OutputIter>, "Just disable these static_asserts and swap active lines below");
//			std::memcpy(std::addressof(*outBegin), std::addressof(*inBegin), std::distance(inBegin, inEnd) * sizeof(T));
			if (outBegin)
				std::memcpy(outBegin, inBegin, std::distance(inBegin, inEnd) * sizeof(T));
		}
		else {
			for (; inBegin != inEnd; ++inBegin, ++outBegin) {
				utl::uninitialized_relocate(std::addressof(*inBegin), std::addressof(*outBegin));
			}
		}
	}
	
	constexpr int popcount(char x) {
		return __builtin_popcount(x);
	}
	constexpr int popcount(short x) {
		return __builtin_popcount(x);
	}
	constexpr int popcount(int x) {
		return __builtin_popcount(x);
	}
	constexpr int popcount(long x) {
		return __builtin_popcountl(x);
	}
	
	constexpr int popcount(unsigned char x) {
		return __builtin_popcount(x);
	}
	constexpr int popcount(unsigned short x) {
		return __builtin_popcount(x);
	}
	constexpr int popcount(unsigned int x) {
		return __builtin_popcount(x);
	}
	constexpr int popcount(unsigned long x) {
		return __builtin_popcountl(x);
	}
	
	constexpr int log2(unsigned long x) {
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_ctzl(x);
#else
		utl_expect(popcount(x) == 1, "x must be a power of 2");
		int result = 0;
		while ((x /= 2) != 0) {
			++result;
		}
		return result;
#endif
	}
	
	constexpr int log2(unsigned char x) {
		return log2(static_cast<unsigned long>(x));
	}
	constexpr int log2(unsigned short x) {
		return log2(static_cast<unsigned long>(x));
	}
	constexpr int log2(unsigned int x) {
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_ctz(x);
#else
		return log2(static_cast<unsigned long>(x));
#endif
	}
	
	
	constexpr int log2(long x) {
		utl_expect(x > 0, "log2 is not defined for x <= 0");
		return log2(static_cast<unsigned long>(x));
	}
	constexpr int log2(char x) {
		return log2(static_cast<long>(x));
	}
	constexpr int log2(short x) {
		return log2(static_cast<long>(x));
	}
	constexpr int log2(int x) {
		return log2(static_cast<long>(x));
	}
	
	template <arithmetic T>
	constexpr T ipow(T base, int exp) {
		utl_expect(exp >= 0);
		return
			exp     == 1 ? base :
			exp     == 0 ? 1 :
			exp % 2 == 0 ? ipow(base * base, exp / 2) :
					base * ipow(base * base, exp / 2);
	}
	
	template <integral T>
	constexpr T fast_mod_pow_two(T x, T y) {
		utl_expect(y >= 0);
		utl_expect(utl::popcount(y) == 1);
		T const e = utl::log2(y);
		return ~(~T{} << e) & x;
	}
	template <integral T, integral U>
	constexpr std::common_type_t<T, U> fast_mod_pow_two(T x, U y) {
		using X = std::common_type_t<T, U>;
		return fast_mod_pow_two((X)x, (X)y);
	}
	
	constexpr char* align_to(void* p, std::size_t align) {
		auto const rest = fast_mod_pow_two((std::uintptr_t)p, align);
		if (!rest) {
			return (char*)p;
		}
		return (char*)p + align - rest;
	}
	
}



namespace utl {
	
	template <unsigned_integral Int>
	class uint_bool_pair {
		static constexpr Int bool_mask = Int(1) << (sizeof(Int) * 8 - 1);
		static constexpr Int int_mask = ~bool_mask;
		
	public:
		uint_bool_pair() = default;
		constexpr uint_bool_pair(Int i, bool b = false): _value(i | (b ? bool_mask : Int(0))) {
			utl_assert(!(i & bool_mask), "too big");
		}
		
		constexpr Int integer() const noexcept {
			return _value & int_mask;
		}
		
		constexpr void integer(Int value) noexcept {
			utl_assert(!(value & bool_mask), "too big");
			_value = value | (boolean() ? bool_mask : Int(0));
		}
		
		constexpr bool boolean() const noexcept {
			return _value & bool_mask;
		}
		
		constexpr void boolean(bool value) noexcept {
			_value = (_value & int_mask) | (value ? bool_mask : Int(0));
		}
		
		constexpr Int raw_value() const noexcept { return _value; }
		
		static constexpr Int max() noexcept { return int_mask; }
		
	private:
		Int _value;
	};
	
	template <typename T, typename = int, std::size_t IntWidth = utl::log2(alignof(T))>
	class pointer_int_pair;
	
	template <typename T, integral Int, std::size_t IntWidth>
	class pointer_int_pair<T*, Int, IntWidth> {
	public:
		static constexpr bool fits(Int i) noexcept {
			if constexpr (std::is_signed_v<Int>) {
				// n bits represent { -2^(n-1), ..., 2^(n-1) - 1 }
				return i >= -((long long)(1) << (int_size - 1)) && i < ((long long)(1) << (int_size - 1));
			}
			else {
				// n bits represent { 0, ..., 2^(n) - 1 }
				return i < ((long long)(1) << (int_size));
			}
		}
		
		pointer_int_pair() = default;
		
		pointer_int_pair(T* p, Int i = 0) {
			utl_expect(fits(i), "integer out of range");
			_p = p;
			_i = i;
		}
		
		T* pointer() const noexcept { return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(_p) & pointer_mask); }
		
		void pointer(T* p) noexcept {
			utl_expect((reinterpret_cast<std::uintptr_t>(p) & ~pointer_mask) == 0, "pointer alignment not satisfied");
			auto const i =_i;
			_p = p;
			_i = i;
		}
		
		auto integer() const noexcept { return _i; }
		
		void integer(Int i) noexcept {
			_i = i;
		}
		
	private:
		static constexpr auto int_size = IntWidth;
		static constexpr unsigned long pointer_mask = ~((long(1) << int_size) - 1);
		union {
			T* _p;
			Int _i : int_size;
		};
	};
	
	
}


