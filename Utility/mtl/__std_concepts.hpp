#pragma once

#include <functional>

#include "__base.hpp"
_MTL_SYSTEM_HEADER_

/// This is an implementation of some standard concepts that are not yet implemented in libc++

#ifndef __MTL_STD_CONCEPTS_HPP_INCLUDED__
#define __MTL_STD_CONCEPTS_HPP_INCLUDED__

#include <type_traits>

namespace _VMTL {

	template <class From, class To>
	concept convertible_to =
		std::is_convertible_v<From, To> &&
		requires {
			static_cast<To>(std::declval<From>());
		};
	
	template <class T, class U>
	concept __mtl_same_helper = std::is_same_v<T, U>;
	
	template <class T, class U>
	concept same_as = __mtl_same_helper<T, U> && __mtl_same_helper<U, T>;

	template <class F, class... Args>
	concept invocable =
		requires(F&& f, Args&&... args) {
			std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
		};
	
	template <class F, class... Args>
	concept regular_invocable = invocable<F, Args...>;
	
	template <class F, class R, class... Args>
	concept invocable_r = invocable<F, Args...> &&
		requires(F&& f, Args&&... args) {
			static_cast<R>(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
		};

	template <class R, class F, class... Args>
	concept regular_invocable_r = invocable_r<R, F, Args...>;
	
}

#endif // __MTL_STD_CONCEPTS_HPP_INCLUDED__
