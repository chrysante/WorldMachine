#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include <type_traits>
#include <iterator>

// MARK: - detectors impl
namespace utl::_private {
	
	template <typename...T>
	void generic_function(T&&...);
	
	template <typename T, typename U>
	constexpr auto has_operator_add(int) -> decltype(std::declval<T>() + std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_add(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_subtract(int) -> decltype(std::declval<T>() - std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_subtract(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_multiply(int) -> decltype(std::declval<T>() * std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_multiply(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_divide(int) -> decltype(std::declval<T>() / std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_divide(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_modulo(int) -> decltype(std::declval<T>() % std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_modulo(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_equals(int) -> decltype(generic_function<bool>(std::declval<T>() == std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_equals(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_less(int) -> decltype(generic_function<bool>(std::declval<T>() < std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_less(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_less_eq(int) -> decltype(generic_function<bool>(std::declval<T>() <= std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_less_eq(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_greater(int) -> decltype(generic_function<bool>(std::declval<T>() > std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_greater(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_greater_eq(int) -> decltype(generic_function<bool>(std::declval<T>() >= std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_greater_eq(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_unequals(int) -> decltype(generic_function<bool>(std::declval<T>() != std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_unequals(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_logical_and(int) -> decltype(generic_function<bool>(std::declval<T>() && std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_logical_and(...) { return false; }
	
	template <typename T>
	constexpr auto has_operator_logical_not(int) -> decltype(!std::declval<T>(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_operator_logical_not(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_bitwise_and(int) -> decltype(generic_function<bool>(std::declval<T>() & std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_bitwise_and(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_bitwise_or(int) -> decltype(generic_function<bool>(std::declval<T>() | std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_bitwise_or(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_bitwise_xor(int) -> decltype(generic_function<bool>(std::declval<T>() ^ std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_bitwise_xor(...) { return false; }
	
	template <typename T>
	constexpr auto has_operator_bitwise_not(int) -> decltype(~std::declval<T>(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_operator_bitwise_not(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_logical_or(int) -> decltype(generic_function<bool>(std::declval<T>() || std::declval<U>()), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_logical_or(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_leftshift(int) -> decltype(std::declval<T>() << std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_leftshift(...) { return false; }
	
	template <typename T, typename U>
	constexpr auto has_operator_rightshift(int) -> decltype(std::declval<T>() >> std::declval<U>(), bool{}) { return true; }
	template <typename, typename>
	constexpr auto has_operator_rightshift(...) { return false; }

	template <typename T>
	void prefix_increment_test(T&& t) { ++t; }
	
	template <typename T>
	constexpr auto has_operator_prefix_increment(int)
		-> decltype(prefix_increment_test(std::declval<T>()), bool{}) { return true; }
	template <typename>
	constexpr auto has_operator_prefix_increment(...) { return false; }
	
	template <typename T>
	constexpr auto has_operator_unary_plus(int) -> decltype(+std::declval<T>(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_operator_unary_plus(...) { return false; }
	
	template <typename T>
	constexpr auto has_operator_unary_minus(int) -> decltype(-std::declval<T>(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_operator_unary_minus(...) { return false; }
	
	template <typename T>
	constexpr auto has_operator_dereference(int) -> decltype(*std::declval<T>(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_operator_dereference(...) { return false; }
	
	template <typename T, typename... Args>
	constexpr auto is_list_initializable(int) -> decltype(T{ std::declval<Args>()... }, bool{}) { return true; }
	template <typename, typename...>
	constexpr auto is_list_initializable(...) { return false; }
	
	
	
}

// MARK: - detectors
namespace utl {
	
	template <typename T, typename U = T>
	struct has_operator_add: std::integral_constant<bool, _private::has_operator_add<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_subtract: std::integral_constant<bool, _private::has_operator_subtract<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_multiply: std::integral_constant<bool, _private::has_operator_multiply<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_divide: std::integral_constant<bool, _private::has_operator_divide<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_modulo: std::integral_constant<bool, _private::has_operator_modulo<T, U>(0)> {};
	
	template <typename T, typename U = T>
	struct has_operator_equals: std::integral_constant<bool, _private::has_operator_equals<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_unequals: std::integral_constant<bool, _private::has_operator_unequals<T, U>(0)> {};
	
	template <typename T, typename U = T>
	struct has_operator_less: std::integral_constant<bool, _private::has_operator_less<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_less_eq: std::integral_constant<bool, _private::has_operator_less_eq<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_greater: std::integral_constant<bool, _private::has_operator_greater<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_greater_eq: std::integral_constant<bool, _private::has_operator_greater_eq<T, U>(0)> {};
	
	template <typename T, typename U = T>
	struct has_operator_logical_and: std::integral_constant<bool, _private::has_operator_logical_and<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_logical_or: std::integral_constant<bool, _private::has_operator_logical_or<T, U>(0)> {};
	template <typename T>
	struct has_operator_logical_not: std::integral_constant<bool, _private::has_operator_logical_not<T>(0)> {};
	
	
	template <typename T, typename U = T>
	struct has_operator_bitwise_and: std::integral_constant<bool, _private::has_operator_bitwise_and<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_bitwise_or: std::integral_constant<bool, _private::has_operator_bitwise_or<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_bitwise_xor: std::integral_constant<bool, _private::has_operator_bitwise_xor<T, U>(0)> {};
	template <typename T>
	struct has_operator_bitwise_not: std::integral_constant<bool, _private::has_operator_bitwise_not<T>(0)> {};
	
	template <typename T, typename U = T>
	struct has_operator_leftshift: std::integral_constant<bool, _private::has_operator_leftshift<T, U>(0)> {};
	template <typename T, typename U = T>
	struct has_operator_rightshift: std::integral_constant<bool, _private::has_operator_rightshift<T, U>(0)> {};
	
	template <typename T>
	struct has_operator_prefix_increment: std::integral_constant<bool, _private::has_operator_prefix_increment<T>(0)> {};
	
	template <typename T>
	struct has_operator_unary_plus: std::integral_constant<bool, _private::has_operator_unary_plus<T>(0)> {};
	template <typename T>
	struct has_operator_unary_minus: std::integral_constant<bool, _private::has_operator_unary_minus<T>(0)> {};
	
	
	template <typename T>
	struct has_operator_dereference: std::integral_constant<bool, _private::has_operator_dereference<T>(0)> {};
	
	template <typename T, typename... Args>
	struct is_list_initializable: std::integral_constant<bool, _private::is_list_initializable<T, Args...>(0)> {};
	
	
	namespace _private {
		template<typename T, bool = std::is_class_v<T>>
		struct _is_any_invocable /* bool = true */ {
			static_assert(!std::is_final_v<T>,
						  "Final classes cannot be used with this facility"
						  "Why do you even make your class final in the fist place?!");
		private:
			struct Fallback { void operator()(); };
			struct Derived : T, Fallback { };

			template<typename U, U> struct Check;

			template<typename>
			static std::true_type test(...);

			template<typename C>
			static std::false_type test(Check<void (Fallback::*)(), &C::operator()>*);

		public:
			static constexpr bool value = decltype(test<Derived>(0))::value;
		};
		template <typename T>
		struct _is_any_invocable<T, false> {
			static constexpr bool value = []{
				if constexpr (std::is_reference_v<T> || std::is_const_v<T>) {
					return _is_any_invocable<std::remove_cvref_t<T>>::value;
				}
				else if constexpr (std::is_scalar_v<T>) {
					return std::is_function_v<std::remove_pointer_t<T>> || std::is_member_pointer_v<T>;
				}
			}();
		};
	}
	
	template <typename T>
	struct is_any_invocable: std::bool_constant<_private::_is_any_invocable<T>::value> {};
	
}

namespace utl {
	namespace _private {
		// from https://stackoverflow.com/questions/12032771/how-to-check-if-an-arbitrary-type-is-an-iterator
		template <typename T>
			struct is_iterator_impl {
				static char test(...);

				template <typename U,
						  typename=typename std::iterator_traits<U>::difference_type,
						  typename=typename std::iterator_traits<U>::pointer,
						  typename=typename std::iterator_traits<U>::reference,
						  typename=typename std::iterator_traits<U>::value_type,
						  typename=typename std::iterator_traits<U>::iterator_category
				> static long test(U&&);
			  

				constexpr static bool value = std::is_same<decltype(test(std::declval<T>())),long>::value;
		};
	}
	
	template <typename T>
	struct is_iterator: std::integral_constant<bool, _private::is_iterator_impl<T>::value>{};
}

namespace utl::_private
{
	template <typename T>
	constexpr auto has_member_begin(int) -> decltype(std::declval<T>().begin(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_member_begin(...) { return false; }
	
	template <typename T>
	constexpr auto has_free_begin(int) -> decltype(begin(std::declval<T>()), bool{}) { return true; }
	template <typename T>
	constexpr auto has_free_begin(...) { return false; }
	
	template <typename T>
	constexpr auto has_begin() { return has_member_begin<T>(0) || has_free_begin<T>(0); }
	
	template <typename T>
	constexpr auto has_member_end(int) -> decltype(std::declval<T>().end(), bool{}) { return true; }
	template <typename T>
	constexpr auto has_member_end(...) { return false; }
	
	template <typename T>
	constexpr auto has_free_end(int) -> decltype(end(std::declval<T>()), bool{}) { return true; }
	template <typename T>
	constexpr auto has_free_end(...) { return false; }

	template <typename T>
	constexpr auto has_end() { return has_member_end<T>(0) || has_free_end<T>(0); }
	
	template <typename Range>
	constexpr auto is_range() {
		if constexpr (has_begin<Range>() && has_end<Range>())
		{
			using T = decltype(std::declval<Range>().begin());
			using U = decltype(std::declval<Range>().end());
			
			if (is_iterator<T>::value && is_iterator<U>::value) {
				return std::is_same<typename std::remove_const<decltype(*std::declval<T>())>::type,
									typename std::remove_const<decltype(*std::declval<U>())>::type>::value;
			}
		}
		return false;
	}
	
}


namespace utl {
	
	template <typename T>
	struct is_range: std::integral_constant<bool, _private::is_range<T>()> {};
	
	template <typename R, bool = utl::is_range<R>::value>
	struct range_traits;
	
	template <typename R>
	struct range_traits<R, true> {
	private:
		static auto _begin() {
			if constexpr (_private::has_member_begin<R>(0)) {
				return std::declval<R>().begin();
			}
			else {
				static_assert(_private::has_free_begin<R>(0));
				return begin(std::declval<R>());
			}
		}
		
		
	public:
		using iterator_type = decltype(_begin());
		using reference_type = decltype(*_begin());
	};
	
	
	
}

namespace utl::_private {
	
	template <typename U, typename... T>
	constexpr bool contains_impl() {
		return (std::is_same<U, T>::value || ...);
	}
	
	template <typename T, typename... U>
	constexpr bool is_unique_impl() {
		if constexpr (sizeof...(U) == 0)
			return true;
		else {
			return ((!std::is_same<T, U>::value) && ...) && is_unique_impl<U...>();
		}
	}
	
	template <std::size_t index, typename U, typename First, typename ... Rest>
	constexpr std::size_t first_index_of_impl() {
		static_assert(contains_impl<U, First, Rest...>());
		if constexpr (std::is_same<U, First>::value)
			return index;
		else
			return first_index_of_impl<index + 1, U, Rest...>();
	}
	
}

namespace utl {
	
	template <typename U, typename... T>
	struct contains: std::integral_constant<bool, _private::contains_impl<U, T...>()> {};
	
	template <typename... T>
	struct is_unique: std::integral_constant<bool, _private::is_unique_impl<T...>()> {};
	
	template <typename U, typename... T>
	struct first_index_of: std::integral_constant<std::size_t, _private::first_index_of_impl<0, U, T...>()> {};
	
	template <typename T>
	inline constexpr bool template_true = std::is_same<T, T>::value;
	
	template <typename T>
	inline constexpr bool template_false = !template_true<T>;
	
}


namespace utl {
	
	template <typename T>
	struct is_arithmetic: std::is_arithmetic<T> {};
	
	template <typename T>
	struct is_scalar: is_arithmetic<T> {};
	
	namespace _private {
		template <typename T, typename U>
		auto _do_promote2(int) -> decltype(bool{} ? std::declval<T>() : std::declval<U>());
		
		template <typename T>
		T _do_promote();
		template <typename T, typename U, typename... Rest>
		auto _do_promote() {
			return _do_promote<std::remove_reference_t<decltype(_do_promote2<T, U>(0))>, Rest...>();
		}
		
		
		
		
		
		template <typename T, typename U>
		auto _can_promote2(int) -> decltype(bool{} ? std::declval<T>() : std::declval<U>(), std::true_type{});
		template <typename T, typename U>
		std::false_type _can_promote2(...);
		template <typename T, typename U, typename... Rest>
		constexpr bool _can_promote(...) {
			if constexpr (decltype(_can_promote2<T, U>(0))::value) {
				if constexpr (sizeof...(Rest) > 0) {
					return _can_promote<std::remove_reference_t<decltype(_do_promote2<T, U>(0))>, Rest...>(0);
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		}
	}
	
	
	template <typename T, typename... U>
	struct promote;
	
	template <typename T, typename U>
	requires(_private::_can_promote<T, U>(0))
	struct promote<T, U> {
		using type = std::remove_reference_t<decltype(_private::_do_promote<T, U>())>;
	};
	
	template <typename T, typename U, typename... V>
	requires(_private::_can_promote<T, U, V...>(0))
	struct promote<T, U, V...> {
		using type = typename promote<typename promote<T, U>::type, V...>::type;
	};
	
	
	
	
	
	template <typename T>
	struct underlying_type { using type = T; };
	
	template <typename... T>
	struct to_floating_point {
		using type = typename promote<typename to_floating_point<T>::type...>::type;
	};
	
	template <typename T>
	struct to_floating_point<T>;
	
	template <> struct to_floating_point<char>               { using type = double; };
	template <> struct to_floating_point<unsigned char>      { using type = double; };
	template <> struct to_floating_point<short>              { using type = double; };
	template <> struct to_floating_point<unsigned short>     { using type = double; };
	template <> struct to_floating_point<int>                { using type = double; };
	template <> struct to_floating_point<unsigned int>       { using type = double; };
	template <> struct to_floating_point<long>               { using type = double; };
	template <> struct to_floating_point<unsigned long>      { using type = double; };
	template <> struct to_floating_point<long long>          { using type = double; };
	template <> struct to_floating_point<unsigned long long> { using type = double; };
	
	template <> struct to_floating_point<float>       { using type = float; };
	template <> struct to_floating_point<double>      { using type = double; };
	template <> struct to_floating_point<long double> { using type = long double; };
	
	template <typename T>
	struct remove_cvref {
		using type = std::remove_cv_t<std::remove_reference_t<T>>;
	};
	
	template <typename T>
	using remove_cvref_t = typename remove_cvref<T>::type;
	
	template <typename T>
	struct is_relocatable: std::conjunction<std::is_move_constructible<T>, std::is_destructible<T>> {};
	
	template <typename T>
	struct is_trivially_relocatable: std::conjunction<std::is_trivially_move_constructible<T>, std::is_trivially_destructible<T>> {};
	
}
