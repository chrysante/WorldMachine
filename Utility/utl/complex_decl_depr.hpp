#pragma once

#include <type_traits>

namespace utl::inline math {
	
	
	template <typename>
	struct complex;
	
	template <typename T>
	struct to_complex { using type = complex<T>; };
	template <typename T>
	struct to_complex<complex<T>> { using type = complex<T>; };
	
	template <typename T>
	struct is_complex: std::false_type {};
	template <typename T>
	struct is_complex<complex<T>>: std::true_type {};
	
}

namespace utl {

	template <typename>
	struct underlying_type;

	template <typename T>
	struct underlying_type<complex<T>> { using type = T; };

}
