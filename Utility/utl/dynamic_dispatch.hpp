#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "debug.hpp"
#include "concepts.hpp"
#include <tuple>
#include <type_traits>

namespace utl {
	
	template <typename... T>
	struct __dispatch_arg_tuple: std::tuple<T...> {
		using std::tuple<T...>::tuple;
	};
	
	template <typename... T>
	__dispatch_arg_tuple(T...) -> __dispatch_arg_tuple<T...>;
	
	template <typename>
	struct __is_dispatch_arg_tuple: std::false_type {};
	template <typename... T>
	struct __is_dispatch_arg_tuple<__dispatch_arg_tuple<T...>>: std::true_type {};
	template <typename... T>
	struct __is_dispatch_arg_tuple<__dispatch_arg_tuple<T...> const>: std::true_type {};
	
	template <typename>
	struct __dispatch_arg;
	
	template <typename... T>
	__dispatch_arg(__dispatch_arg_tuple<T...>, std::size_t) -> __dispatch_arg<__dispatch_arg_tuple<T...>>;
	template <typename... T>
	struct __dispatch_arg<__dispatch_arg_tuple<T...>> {
		__dispatch_arg(__dispatch_arg_tuple<T...> x, std::size_t i): x(x), index(i) {}
		__dispatch_arg_tuple<T...> x;
		std::size_t index;
		
		using __test_type = std::tuple_element_t<0, std::tuple<T...>>;
	};
	
	template <std::size_t N>
	__dispatch_arg(std::integral_constant<std::size_t, N>, std::size_t)
		-> __dispatch_arg<std::integral_constant<std::size_t, N>>;
	
	template <std::size_t N>
	struct __dispatch_arg<std::integral_constant<std::size_t, N>> {
		__dispatch_arg(std::integral_constant<std::size_t, N> x, std::size_t i): x(x), index(i) {}
		std::integral_constant<std::size_t, N> x;
		std::size_t index;
		
		using __test_type = std::integral_constant<std::size_t, 0>;
	};
	
	struct __dispatch_bool_tag{};
	
	__dispatch_arg(bool) -> __dispatch_arg<__dispatch_bool_tag>;
	
	template <>
	struct __dispatch_arg<__dispatch_bool_tag> {
		__dispatch_arg(bool value): x(), index(value) {}
		std::integral_constant<std::size_t, 2> x;
		std::size_t index;
		
		using __test_type = std::false_type;
	};
	
	template <std::size_t I, typename... T>
	auto const& get(__dispatch_arg_tuple<T...> const& t) {
		return std::get<I>(static_cast<std::tuple<T...> const&>(t));
	}
	template <std::size_t I, typename... T>
	auto& get(__dispatch_arg_tuple<T...>& t) {
		return as_mutable(get<I>(as_const(t)));
	}
	
	template <typename T>
	struct __is_integral_constant: std::false_type {};
	template <typename T, T Value>
	struct __is_integral_constant<std::integral_constant<T, Value>>: std::true_type {};
	template <typename T, T Value>
	struct __is_integral_constant<std::integral_constant<T, Value> const>: std::true_type {};
	
	template <typename...T>
	void __dispatch_one_impl(auto&& selector, __dispatch_arg_tuple<T...> const& t, std::size_t index, auto&&... args) {
		auto constexpr size = sizeof...(T);
		utl_bounds_check(index, 0, size);
		UTL_WITH_INDEX_SEQUENCE((I, size), {
			
			auto invoke = [&]<std::size_t J>(std::integral_constant<std::size_t, J>){
				selector(std::get<J>(t), UTL_FORWARD(args)...);
			};
			
			(void)(... || (I == index ? (invoke(std::integral_constant<std::size_t, I>{}), true) : false));
		});
	}

	void __dispatch_one_impl(auto&& selector, auto size_constant, std::size_t index, auto&&... args)
	requires requires{ selector(size_constant); } && __is_integral_constant<decltype(size_constant)>::value
	{
		constexpr std::size_t size = decltype(size_constant)::value;
		utl_bounds_check(index, 0, size);
		UTL_WITH_INDEX_SEQUENCE((I, size), {
			
			auto invoke = [&]<std::size_t J>(std::integral_constant<std::size_t, J>){
				selector(std::integral_constant<std::size_t, J>{}, UTL_FORWARD(args)...);
			};
			
			(void)(... || (I == index ? (invoke(std::integral_constant<std::size_t, I>{}), true) : false));
		});
	}
	
	// recursion base case
	template <std::size_t Size, typename... Args>
	void __dispatch_impl(std::integral_constant<std::size_t, Size - 1>, auto&& selector, __dispatch_arg_tuple<Args...> const& argTuple, auto&&... x)
	{
		std::size_t constexpr Index = Size - 1;
		__dispatch_one_impl([&](auto toUnpack, auto... x){
			
			constexpr bool viable =
				!__is_dispatch_arg_tuple<std::decay_t<decltype(toUnpack)>>::value &&
				((!__is_dispatch_arg_tuple<std::decay_t<decltype(x)>>::value) && ...);
			
			if constexpr (sizeof...(x) + 1 == Size && viable) {
				selector(x..., toUnpack);
			}
		}, get<Index>(argTuple).x, get<Index>(argTuple).index, x...);
	}

	// general case
	template <std::size_t Size, std::size_t Index, typename... Args>
	void __dispatch_impl(std::integral_constant<std::size_t, Index>, auto&& selector, __dispatch_arg_tuple<Args...> const& argTuple, auto&&... x) {
		static_assert(Index < Size - 1);
		__dispatch_one_impl([&](auto toUnpack, auto... x){
			__dispatch_impl<Size>(std::integral_constant<std::size_t, Index + 1>{}, selector, argTuple, x..., toUnpack);
		}, get<Index>(argTuple).x, get<Index>(argTuple).index, x...);
	}

	// MARK: - Interface
	template <typename ...T>
	auto dispatch_arg(std::size_t index, T&&... args) {
		utl_bounds_check(index, 0, sizeof...(T));
		return __dispatch_arg{ __dispatch_arg_tuple{ UTL_FORWARD(args)... }, index };
	}

	template <std::size_t N>
	auto dispatch_range(std::size_t index) {
		utl_bounds_check(index, 0, N);
		return __dispatch_arg{ std::integral_constant<std::size_t, N>{}, index };
	}
	
	inline auto dispatch_condition(bool value) {
		return __dispatch_arg{ value };
	}

	template <typename ... T>
	void __dispatch(__dispatch_arg_tuple<__dispatch_arg<T>...> const& args, auto&& selector) {
		__dispatch_impl<sizeof...(T)>(std::integral_constant<std::size_t, 0>{}, UTL_FORWARD(selector), args);
	}

	template <class T>
	void dispatch(__dispatch_arg<T> const& a,
				  utl::invocable<typename __dispatch_arg<T>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a }, UTL_FORWARD(selector));
	}
	template <class T, class U>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b }, UTL_FORWARD(selector));
	}
	template <class T, class U, class V>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  __dispatch_arg<V> const& c,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type,
								 typename __dispatch_arg<V>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b, c }, UTL_FORWARD(selector));
	}
	template <class T, class U, class V, class W>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  __dispatch_arg<V> const& c,
				  __dispatch_arg<W> const& d,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type,
								 typename __dispatch_arg<V>::__test_type,
								 typename __dispatch_arg<W>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b, c, d }, UTL_FORWARD(selector));
	}
	template <class T, class U, class V, class W, class X>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  __dispatch_arg<V> const& c,
				  __dispatch_arg<W> const& d,
				  __dispatch_arg<X> const& e,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type,
								 typename __dispatch_arg<V>::__test_type,
								 typename __dispatch_arg<W>::__test_type,
								 typename __dispatch_arg<X>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b, c, d, e }, UTL_FORWARD(selector));
	}
	template <class T, class U, class V, class W, class X, class Y>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  __dispatch_arg<V> const& c,
				  __dispatch_arg<W> const& d,
				  __dispatch_arg<X> const& e,
				  __dispatch_arg<Y> const& f,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type,
								 typename __dispatch_arg<V>::__test_type,
								 typename __dispatch_arg<W>::__test_type,
								 typename __dispatch_arg<X>::__test_type,
								 typename __dispatch_arg<Y>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b, c, d, e, f }, UTL_FORWARD(selector));
	}
	template <class T, class U, class V, class W, class X, class Y, class Z>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  __dispatch_arg<V> const& c,
				  __dispatch_arg<W> const& d,
				  __dispatch_arg<X> const& e,
				  __dispatch_arg<Y> const& f,
				  __dispatch_arg<Z> const& g,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type,
								 typename __dispatch_arg<V>::__test_type,
								 typename __dispatch_arg<W>::__test_type,
								 typename __dispatch_arg<X>::__test_type,
								 typename __dispatch_arg<Y>::__test_type,
								 typename __dispatch_arg<Z>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b, c, d, e, f, g }, UTL_FORWARD(selector));
	}
	template <class T, class U, class V, class W, class X, class Y, class Z, class A>
	void dispatch(__dispatch_arg<T> const& a,
				  __dispatch_arg<U> const& b,
				  __dispatch_arg<V> const& c,
				  __dispatch_arg<W> const& d,
				  __dispatch_arg<X> const& e,
				  __dispatch_arg<Y> const& f,
				  __dispatch_arg<Z> const& g,
				  __dispatch_arg<A> const& h,
				  utl::invocable<typename __dispatch_arg<T>::__test_type,
								 typename __dispatch_arg<U>::__test_type,
								 typename __dispatch_arg<V>::__test_type,
								 typename __dispatch_arg<W>::__test_type,
								 typename __dispatch_arg<X>::__test_type,
								 typename __dispatch_arg<Y>::__test_type,
								 typename __dispatch_arg<Z>::__test_type,
								 typename __dispatch_arg<A>::__test_type> auto&& selector) {
		__dispatch(__dispatch_arg_tuple{ a, b, c, d, e, f, g, h }, UTL_FORWARD(selector));
	}
	
}
