#pragma once

#include "vector_math.hpp"
#include "complex.hpp"

namespace utl::inline math {
	
	namespace math_internal {
		template <typename T, int N, vector_options O>
		constexpr std::array<vec<T, N / 2, O>, 2> splice(vec<T, N, O> const& v) {
			static_assert(N % 2 == 0);
			return {
				vec<T, N / 2, O>([&](auto i) { return v[i * 2]; }),
				vec<T, N / 2, O>([&](auto i) { return v[i * 2 + 1]; })
			};
		}
		
		template <bool Inverse, int N, typename T, vector_options O = default_vector_options>
		vec<complex<T>, N, O> fft_impl(vec<complex<T>, N, O> const& c) {
			static_assert(is_power_of_two(N));
			if constexpr (N == 1) {
				return c;
			}
			else {
				auto const [d, e] = splice(c);
				auto const g = fft_impl<Inverse>(d);
				auto const h = fft_impl<Inverse>(e);
				
				auto const omega = utl::expi((Inverse * 2 - 1) * 2 * utl::constants<T>::pi / N);
				vec<complex<T>, N / 2, O> const w([&](auto i) { return pow(omega, i); });
				
				auto const wh = w * h;
				
				return append(g + wh, g - wh);
			}
		}
	}
	
	template <int N, typename T, vector_options O = default_vector_options>
	auto fft(vec<T, N, O> const& c) {
		return fft(vec<complex<T>, N, O>(c));
	}
	
	template <int N, typename T, vector_options O = default_vector_options>
	vec<complex<T>, N, O> fft(vec<complex<T>, N, O> const& c) {
		static_assert(is_power_of_two(N));
		return math_internal::fft_impl<false>(c);
	}
	
	template <int N, typename T, vector_options O = default_vector_options>
	vec<complex<T>, N, O> inverse_fft(vec<complex<T>, N, O> const& c) {
		static_assert(is_power_of_two(N));
		return math_internal::fft_impl<true>(c) / N;
	}
	
	
	
}
