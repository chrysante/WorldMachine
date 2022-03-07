#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

namespace utl::_private {
	
	enum struct range_type {
		invalid,
		integral,
		floating_point
	};
	
	template <typename T>
	constexpr range_type get_range_type() {
		if constexpr (std::is_integral<T>::value) {
			return range_type::integral;
		}
		else if constexpr (std::is_floating_point<T>::value) {
			return range_type::floating_point;
		}
		else {
			return range_type::invalid;
		}
	}
	
}

namespace utl {

	template <typename T = std::size_t, _private::range_type = _private::get_range_type<T>()>
	class range;
	
	template <typename T>
	range(T) -> range<T>;
	
	template <typename T>
	class range<T, _private::range_type::integral> {
	public:
		class iterator {
		public:
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;
			using value_type = T;
			using iterator_category = std::random_access_iterator_tag;
			
			explicit constexpr iterator(T value) noexcept: m_value(value) {}
			
			constexpr iterator& operator++() noexcept { ++m_value; return *this; }
			
			constexpr T operator+() const noexcept { return value(); }
			constexpr T value() const noexcept { return m_value; }
			constexpr operator T() const noexcept { return value(); }
			constexpr T operator*() const noexcept { return value(); }
			
			friend constexpr bool operator==(iterator a, iterator b) noexcept {
				return a.value() == b.value();
			}
			
			friend constexpr bool operator!=(iterator a, iterator b) noexcept {
				return !(a == b);
			}
			
		private:
			T m_value;
		};
		
		static_assert(is_iterator<iterator>::value);
		
	public:
		explicit constexpr range(T end) noexcept: m_begin(0), m_end(end) {}
		explicit constexpr range(T begin, T end) noexcept: m_begin(begin), m_end(end) {}
	
		template <typename U>
		explicit constexpr operator range<U>() const noexcept { return range<U>(static_cast<U>(this->m_begin), static_cast<U>(this->m_end)); }
		
		constexpr iterator begin() const noexcept { return iterator(m_begin); }
		constexpr iterator end() const noexcept   { return iterator(m_end); }
		
	private:
		T m_begin, m_end;
	};
	
}
