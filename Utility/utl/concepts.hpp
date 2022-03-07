#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <type_traits>
#include <utility>
#include <functional>

namespace utl {
	
	template <typename T>
	concept boolean_testable = requires(T&& t) {  static_cast<bool>(t); };

	template <typename T>
	concept arithmetic = std::is_arithmetic_v<T>;
	
	template <typename T>
	concept integral = std::is_integral_v<T>;
	
	template <typename T>
	concept signed_integral = std::is_signed_v<T>;
	
	template <typename T>
	concept unsigned_integral = std::is_unsigned_v<T>;
	
	template <typename T>
	concept floating_point = std::is_floating_point_v<T>;
	
	template <class From, class To>
	concept convertible_to =
		std::is_convertible_v<From, To> &&
		requires {
			static_cast<To>(std::declval<From>());
		};
	
	template <class F, class... Args>
	concept invocable =
		requires(F&& f, Args&&... args) {
			std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
		};
	
	template <typename Derived, typename Base>
	concept derived_from =
	  std::is_base_of_v<Base, Derived> &&
	  std::is_convertible_v<const volatile Derived*, const volatile Base*>;
	
	template <class T, class U>
	concept __same_helper = std::is_same_v<T, U>;
	 
	template <class T, class U>
	concept same_as = __same_helper<T, U> && __same_helper<U, T>;
			
	template <typename T, typename U, typename ... V>
	concept any_of = (same_as<T, U> || ... || same_as<T, V>);
	
	template <class F, class... Args>
	concept regular_invocable = invocable<F, Args...>;
	
	template <class F, class R, class... Args>
	concept invocable_r = invocable<F, Args...> &&
		requires(F&& f, Args&&... args) {
			static_cast<R>(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
		};
	
	template <class R, class F, class... Args>
	concept regular_invocable_r = invocable_r<R, F, Args...>;
	
	template <typename I>
	concept input_iterator = requires(I&& i) {
		{ i != i } -> convertible_to<bool>;
		{ *i }     -> convertible_to<typename std::iterator_traits<I>::value_type>;
		{ ++i }    -> same_as<I&>;
	};
	
	template <typename I, typename T>
	concept input_iterator_for = input_iterator<I> && requires(I&& i) {
		{ *i } -> convertible_to<T>;
	};
	
//	template <typename> struct is_floating_point: std::false_type{};
//	template <> struct is_floating_point<float>: std::true_type{};
//	template <> struct is_floating_point<double>: std::true_type{};
//	template <> struct is_floating_point<long double>: std::true_type{};
//	template <typename T> struct is_floating_point<complex<T>>: is_floating_point<T>{};
	
//	template <class T, class U>
//	concept common_reference_with =
//	  same_as<std::common_reference_t<T, U>, std::common_reference_t<U, T>> &&
//	  std::convertible_to<T, std::common_reference_t<T, U>> &&
//	  std::convertible_to<U, std::common_reference_t<T, U>>;
//	
//	namespace _private {
//		template<class T, class U>
//		concept WeaklyEqualityComparableWith = // exposition only
//			requires(const std::remove_reference_t<T>& t,
//					 const std::remove_reference_t<U>& u) {
//				{ t == u } -> boolean_testable;
//				{ t != u } -> boolean_testable;
//				{ u == t } -> boolean_testable;
//				{ u != t } -> boolean_testable;
//			};
//	}
//	
//	template <class T>
//	concept equality_comparable = _private::WeaklyEqualityComparableWith<T, T>;
//	
//	template <class T, class U>
//	concept equality_comparable_with =
//		equality_comparable<T> &&
//		equality_comparable<U> &&
//		std::common_reference_with<
//			const std::remove_reference_t<T>&,
//			const std::remove_reference_t<U>&> &&
//		equality_comparable<
//			std::common_reference_t<
//				const std::remove_reference_t<T>&,
//				const std::remove_reference_t<U>&>> &&
//		::WeaklyEqualityComparableWith<T, U>;
		
	
	
}





