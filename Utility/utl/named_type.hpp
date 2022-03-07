#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "type_traits.hpp"

namespace utl::_private {
	enum struct named_type_availability {
		unavailable = 0,
		inheritance = 1,
		conversion = 2
	};
	
	template <typename T>
	constexpr named_type_availability named_type_available() {
		if constexpr (std::is_fundamental<T>::value || std::is_pointer<T>::value || std::is_union<T>::value) {
			return named_type_availability::conversion;
		}
		else if constexpr (std::is_class<T>::value) {
			return named_type_availability::inheritance;
		}
		else if constexpr (std::is_enum<T>::value) {
			return named_type_availability::conversion;
		}
		else {
			return named_type_availability::unavailable;
		}
	}
	
}

namespace utl {
	
	template <typename T, typename Name,
			  _private::named_type_availability = _private::named_type_available<T>()>
	struct named_type;
	
	template <typename T, typename Name>
	struct named_type<T, Name, _private::named_type_availability::inheritance>: public T {
	public:
		template <typename... Args, UTL_ENABLE_IF(std::is_constructible<T, Args...>::value)>
		constexpr named_type<T, Name, _private::named_type_availability::inheritance>(Args&&... args): T(std::forward<Args>(args)...) {}
		constexpr named_type<T, Name, _private::named_type_availability::inheritance>(T const& value): T(value) {}
		constexpr named_type<T, Name, _private::named_type_availability::inheritance>(T&& value) : T(std::move(value)) {}
	};
	
	template <typename T, typename Name>
	struct named_type<T, Name, _private::named_type_availability::conversion> {
	public:
		// default constructors
		named_type<T, Name, _private::named_type_availability::conversion>() = default;
		constexpr named_type<T, Name, _private::named_type_availability::conversion>(T value) noexcept: m_value(value) {}
		
		// union constructor
		template <typename U,
				  UTL_ENABLE_IF(is_list_initializable<T, U>::value),
				  UTL_ENABLE_MEMBER_IF(std::is_union<T>::value)>
		constexpr named_type<T, Name, _private::named_type_availability::conversion>(U&& u): m_value{ std::forward<U>(u) } {}
		
		// union member access
		template <UTL_ENABLE_MEMBER_IF(std::is_union<T>::value)>
		constexpr T* operator->() noexcept { return &m_value; }
		constexpr T const* operator->() const noexcept { return &m_value; }
		
		// get
		constexpr T& get() { return m_value; }
		constexpr T const& get() const { return m_value; }
		
		// implicit conversion
		constexpr operator T&() noexcept { return get(); }
		constexpr operator T const&() const noexcept { return get(); }
		
		// explicit conversion (constructor)
		template <typename U, UTL_ENABLE_IF(std::is_constructible<T, U>::value)>
		constexpr explicit named_type<T, Name, _private::named_type_availability::conversion>(U u) noexcept: m_value(static_cast<T>(u)) {}
		template <typename U, UTL_ENABLE_IF(std::is_constructible<U, T>::value)>
		constexpr explicit operator U() const { return static_cast<U>(get()); }
		
	private:
		T m_value;
	};
	
	template <typename T, typename Name, UTL_ENABLE_IF(has_operator_bitwise_and<T>::value)>
	constexpr named_type<T, Name> operator&(named_type<T, Name> const& a, named_type<T, Name> const& b) {
		return a.get() & b.get();
	}
	
	template <typename T, typename Name, UTL_ENABLE_IF(has_operator_bitwise_or<T>::value)>
	constexpr named_type<T, Name> operator|(named_type<T, Name> const& a, named_type<T, Name> const& b) {
		return a.get() | b.get();
	}
	
	template <typename T, typename Name, UTL_ENABLE_IF(has_operator_bitwise_xor<T>::value)>
	constexpr named_type<T, Name> operator^(named_type<T, Name> const& a, named_type<T, Name> const& b) {
		return a.get() ^ b.get();
	}
	
	template <typename T, typename Name, UTL_ENABLE_IF(has_operator_bitwise_not<T>::value)>
	constexpr named_type<T, Name> operator~(named_type<T, Name> const& a) {
		return ~a.get();
	}
	
	template <typename T, typename Name, UTL_ENABLE_IF(has_operator_logical_not<T>::value)>
	constexpr auto operator!(named_type<T, Name> const& a) {
		return !a.get();
	}
	
	
}
