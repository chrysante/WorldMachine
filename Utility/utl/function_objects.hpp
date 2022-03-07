#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "type_traits.hpp"
#include <cmath>
#include "math_util.hpp"
#include "concepts.hpp"

#define UTL_INTERNAL_FUNCTION_OBJECT_DEF(obj_name, impl) \
struct obj_name##_t { \
	UTL(NODISCARD) constexpr auto operator() impl \
} inline constexpr obj_name{}


#define UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(obj_name, operator_name, op, extendDef) \
struct obj_name##_t { \
	template <typename T, typename U> \
	requires(has_operator_##operator_name<T, U>::value) \
	UTL(NODISCARD) constexpr auto operator()(T&& a, U&& b) const { \
		return UTL_FORWARD(a) op UTL_FORWARD(b); \
	} \
	template <class T, class U, class... V, UTL_ENABLE_MEMBER_IF(extendDef)> \
	UTL(NODISCARD) constexpr auto operator()(T&& a, U&& b, V&&... c) const { \
		auto result = (a op b); \
		((result = (result op c)), ...); \
		return result; \
	} \
} inline constexpr obj_name{}




namespace utl {
	struct plus_t {
		template <typename T>
		requires requires(T&& t) { +t; }
		UTL(NODISCARD) constexpr auto operator()(T&& a) const {
			return +UTL_FORWARD(a);
		}
		template <typename T, typename U>
		requires requires(T&& t, U&& u) { t + u; }
		UTL(NODISCARD) constexpr auto operator()(T&& a, U&& b) const {
			return UTL_FORWARD(a) + UTL_FORWARD(b);
		}
		template <class T, class U, class... V>
		UTL(NODISCARD) constexpr auto operator()(T&& a, U&& b, V&&... c) const {
			auto result = (a + b);
			((result = (result + c)), ...);
			return result;
		}
	} inline constexpr plus{};
	struct minus_t {
		template <typename T>
		requires requires(T&& t) { -t; }
		UTL(NODISCARD) constexpr auto operator()(T&& a) const {
			return -UTL_FORWARD(a);
		}
		template <typename T, typename U>
		requires requires(T&& t, U&& u) { t - u; }
		UTL(NODISCARD) constexpr auto operator()(T&& a, U&& b) const {
			return UTL_FORWARD(a) - UTL_FORWARD(b);
		}
	} inline constexpr minus{};
	
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(multiplies,  multiply,     *, true);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(divides,     divide,       /, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(modulo,      modulo,       %, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(equals,      equals,      ==, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(less,        less,         <, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(less_eq,     less_eq,     <=, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(greater,     greater,      >, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(greater_eq,  greater_eq,  >=, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(unequals,    unequals,    !=, false);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(logical_and, logical_and, &&, true);
	UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF(logical_or,  logical_or,  ||, true);
	
#undef UTL_INTERNAL_BIN_FUNCTION_OBJECT_DEF
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(abs, (auto const& x) const {
		using ::std::abs;
		using namespace utl::_private;
		return abs(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(sqrt, (auto const& x) const {
		using ::std::sqrt;
		using namespace utl::_private;
		return sqrt(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(signed_sqrt, (auto const& x) const {
		using namespace utl::_private;
		return signed_sqrt(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(signed_pow, (auto const& x, auto const& y) const {
		using namespace utl::_private;
		return signed_pow(x, y);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(exp, (auto const& x) const {
		using ::std::exp;
		return exp(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(min, (auto const& x, auto const& y) const {
		using ::std::min;
		return min(x, y);
	});
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(max, (auto const& x, auto const& y) const {
		using ::std::max;
		return max(x, y);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(screen_blend, (auto const& x, auto const& y) const {
		return 1 - (1 - x) * (1 - y);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(one_minus, (auto const& x) const {
		return 1 - x;
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(ceil_divide, (auto const& x, auto const& y) const {
		using namespace utl::_private;
		return ceil_divide(x, y);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(floor, (auto const& x) const {
		using namespace utl::_private;
		return floor(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(ceil, (auto const& x) const {
		using namespace utl::_private;
		return ceil(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(fract, (auto const& x) const {
		using namespace utl::_private;
		return fract(x);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(mod, (auto const& x, auto const& y) const {
		using namespace utl::_private;
		return mod(x, y);
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(is_positive, (auto const& x) const {
		return x > 0;
	});
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(is_non_positive, (auto const& x) const {
		return x <= 0;
	});
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(is_negative, (auto const& x) const {
		return x < 0;
	});
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(is_non_negative, (auto const& x) const {
		return x >= 0;
	});
	
	UTL_INTERNAL_FUNCTION_OBJECT_DEF(identity, (auto&& x) const {
		return UTL_FORWARD(x);
	});
	
	
	
	
	namespace _private {
	
		template <typename ... F>
		requires(sizeof...(F) > 0)
		class _compose {
		private:
			template <std::size_t Index, typename... T>
			auto _invoke_impl(T&& ... t) const {
				if constexpr (Index == 0) {
					return std::invoke(std::get<Index>(_f), UTL_FORWARD(t)...);
				}
				else {
					return _invoke_impl<Index - 1>(std::invoke(std::get<Index>(_f), UTL_FORWARD(t)...));
				}
				
			}
			
			template <std::size_t Index, typename T>
			static constexpr bool _is_invocable_rec() {
				if constexpr (Index == 0) {
					return true;
				}
				else {
					using Fn = std::tuple_element_t<Index - 1, std::tuple<F...>>;
					if constexpr (!std::is_invocable_v<Fn, T>) {
						return false;
					}
					else {
						return _is_invocable_rec<Index - 1, std::invoke_result_t<Fn, T>>();
					}
				}
			}
			template <typename ... T>
			static constexpr bool _is_invocable() {
				using LastFN = std::tuple_element_t<sizeof...(F) - 1, std::tuple<F...>>;
				if constexpr (!std::is_invocable_v<LastFN, T...>) {
					return false;
				}
				else {
					return _is_invocable_rec<sizeof...(F) - 1, std::invoke_result_t<LastFN, T...>>();
				}
			}
			
			
		public:
			explicit constexpr _compose(F... f): _f(std::move(f)...) {}
			
			template <typename... T>
			requires(_is_invocable<T...>())
			constexpr auto operator()(T&& ... t) const {
				return _invoke_impl<sizeof...(F) - 1>(UTL_FORWARD(t)...);
			}
			
		private:
			std::tuple<F...> _f;
		};
		
		template <typename ... F>
		requires(sizeof...(F) > 0)
		class _fappend {
			template <std::size_t J>
			using Fn = std::tuple_element_t<J, std::tuple<F...>>;
			template <std::size_t J>
			using Fn = std::tuple_element_t<J, std::tuple<F...>>;
			
		public:
			explicit constexpr _fappend(F... f): _f(std::move(f)...) {}
			
			template <typename... T>
			requires((std::is_invocable_v<F, T...> && ...))
			constexpr auto operator()(T&& ... t) const {
				return UTL_WITH_INDEX_SEQUENCE((I, sizeof...(F)), {
					if constexpr ((... && !std::is_same_v<std::invoke_result_t<Fn<I>, T...>, void>)) {
						return std::tuple(std::invoke(std::get<I>(_f), t...)...);
					}
					else {
						(std::invoke(std::get<I>(_f), t...), ...);
					}
				});
			}
			
		private:
			std::tuple<F...> _f;
		};
	}
	
	/*
	 * compose(f, g, h) returns f * g * h: (...) -> f(g(h(...)))
	 */
	template <typename... F>
	requires(sizeof...(F) > 0 && (is_any_invocable<F>::value && ...))
	UTL(NODISCARD) constexpr _private::_compose<std::remove_cvref_t<F>...> compose(F&&... f) {
		return _private::_compose<std::remove_cvref_t<F>...>(UTL_FORWARD(f)...);
	}
	
	/*
	 * append(f, g, h) returns (f, g, h): (...) -> (f(...), g(...), h(...))
	 */
	template <typename... F>
	requires(sizeof...(F) > 0 && (is_any_invocable<F>::value && ...))
	UTL(NODISCARD) constexpr _private::_fappend<std::remove_cvref_t<F>...> fappend(F&&... f) {
		return _private::_fappend<std::remove_cvref_t<F>...>(UTL_FORWARD(f)...);
	}

}

