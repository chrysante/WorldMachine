#pragma once


#include "base.hpp" // for UTL(CPP) / UTL(C)
UTL(SYSTEM_HEADER)

// MARK: - assert
/**
 * if UTL(DEBUG_LEVEL) is defined as 0 or undefined, no assertions are checked
 * if UTL(DEBUG_LEVEL) is defined as >= 1, '_default' assertions are checked
 * if UTL(DEBUG_LEVEL) is defined as >= 2, '_default' and 'audit' assertions are checked
 */

#if UTL(DEBUG_LEVEL) > 1
#define utl_assert(...) UTL_ASSERT_IMPL("UTL", UTL(DEBUG_LEVEL), assertion, __VA_ARGS__)
#define utl_expect(...) UTL_ASSERT_IMPL("UTL", UTL(DEBUG_LEVEL), precondition, __VA_ARGS__)
#define utl_ensure(...) UTL_ASSERT_IMPL("UTL", UTL(DEBUG_LEVEL), postcondition, __VA_ARGS__)
#define utl_bounds_check(index, lower, upper) UTL_BOUNDS_CHECK_IMPL("UTL", UTL(DEBUG_LEVEL), index, lower, upper)
#else // UTL(DEBUG_LEVEL) > 1
#define utl_assert(...)
#define utl_expect(...)
#define utl_ensure(...)
#define utl_bounds_check(index, lower, upper)
#endif // UTL(DEBUG_LEVEL) > 1

#define UTL_BOUNDS_CHECK_IMPL(ModuleName, DebugLevel, index, lower, upper) \
UTL_ASSERT_IMPL(ModuleName, DebugLevel, precondition, lower <= index, "Index below lower bound"); \
UTL_ASSERT_IMPL(ModuleName, DebugLevel, precondition, index <  upper, "Index above upper bound")

#define utl_debugbreak(msg) UTL_INTERNAL_BREAK(msg)

#define UTL_INTERNAL_DEBUG_WRAP_VALUE(...) [&]{ \
	return __VA_ARGS__; \
}
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(...) [&]{ \
	using ::utl::_private::debug::_default; \
	using ::utl::_private::debug::audit; \
	using ::utl::_private::debug::trace; \
	using ::utl::_private::debug::info; \
	using ::utl::_private::debug::debug; \
	using ::utl::_private::debug::warning; \
	using ::utl::_private::debug::error; \
	using ::utl::_private::debug::fatal; \
	return __VA_ARGS__; \
}
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_1(x) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(x)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_2(x, y) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(x), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(y)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_3(x, y, z) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(x), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(y), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(z)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_4(_1, _2, _3, _4) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_5(_1, _2, _3, _4, _5) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_6(_1, _2, _3, _4, _5, _6) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_7(_1, _2, _3, _4, _5, _6, _7) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_8(_1, _2, _3, _4, _5, _6, _7, _8) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_8)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_8), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_9)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_8), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_9), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_10)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_8), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_9), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_10), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_11)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_8), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_9), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_10), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_11), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_12)
#define UTL_INTERNAL_DEBUG_WRAP_VALUE_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_1), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(_2), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_3), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_4), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_5), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_6), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_7), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_8), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_9), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_10), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_11), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_12), \
	UTL_INTERNAL_DEBUG_WRAP_VALUE(_13)

#define UTL_INTERNAL_DEBUG_WRAP_VALUE_N(...) VFUNC(UTL_INTERNAL_DEBUG_WRAP_VALUE_, __VA_ARGS__)

#define UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(x) UTL_TO_STRING(x)
#define UTL_INTERNAL_ASSERT_STRINGIFY_VALUE_1(x) UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(x)
#define UTL_INTERNAL_ASSERT_STRINGIFY_VALUE_2(x, y) UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(x), UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(y)
#define UTL_INTERNAL_ASSERT_STRINGIFY_VALUE_3(x, y, z) UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(x), UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(y), UTL_INTERNAL_ASSERT_STRINGIFY_VALUE(z)
#define UTL_INTERNAL_ASSERT_STRINGIFY_VALUE_N(...) VFUNC(UTL_INTERNAL_ASSERT_STRINGIFY_VALUE_, __VA_ARGS__)

#define UTL_INTERNAL_ASSERT_PARSE_EXPR(x) \
UTL_INTERNAL_DEBUG_WRAP_VALUE(UTL_PARSE_EXPRESSION(x))

#define UTL_INTERNAL_ASSERT_PARSE_EXPR_USING_DECL(x) \
UTL_INTERNAL_DEBUG_WRAP_VALUE_USING_DECL(UTL_PARSE_EXPRESSION(x))

#define UTL_INTERNAL_ASSERT_PARSE_EXPR_1(x) UTL_INTERNAL_ASSERT_PARSE_EXPR_USING_DECL(x)
#define UTL_INTERNAL_ASSERT_PARSE_EXPR_2(x, y) UTL_INTERNAL_ASSERT_PARSE_EXPR_USING_DECL(x), UTL_INTERNAL_ASSERT_PARSE_EXPR_USING_DECL(y)
#define UTL_INTERNAL_ASSERT_PARSE_EXPR_3(x, y, z) UTL_INTERNAL_ASSERT_PARSE_EXPR_USING_DECL(x), UTL_INTERNAL_ASSERT_PARSE_EXPR_USING_DECL(y), UTL_INTERNAL_ASSERT_PARSE_EXPR(z)
#define UTL_INTERNAL_ASSERT_PARSE_EXPR_N(...) VFUNC(UTL_INTERNAL_ASSERT_PARSE_EXPR_, __VA_ARGS__)

// MARK: - log

#define utl_log(...) UTL_LOG_IMPL("UTL", UTL(DEBUG_LEVEL), __VA_ARGS__)


#define UTL_LOG_IMPL(module_name, debug_level, ...) ::utl::_private::debug::_log_<debug_level>(UTL_INTERNAL_MAKE_LOG_DATA(module_name), \
																							   UTL_INTERNAL_DEBUG_WRAP_VALUE_N(__VA_ARGS__))


#define UTL_INTERNAL_MAKE_LOG_DATA(module_name) ::utl::_private::debug::log_data{ module_name, __PRETTY_FUNCTION__, __FILE__, __LINE__ }

#ifdef UTL_DEBUG_LEVEL
#define UTL_PRIVATE_DEFINITION_DEBUG_LEVEL() UTL_DEBUG_LEVEL
#else // UTL_DEBUG_LEVEL
#define UTL_PRIVATE_DEFINITION_DEBUG_LEVEL() 0
#endif // UTL_DEBUG_LEVEL

// debug-break
//#if UTL(DEBUG_LEVEL) > 0

#if UTL(CPP)

#include "expression_parser.hpp"

#include <type_traits>
#include "format.hpp"

#if UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS

#include <stdexcept>
namespace utl::_private::debug { void _throw_runtime_error(); }
#define UTL_INTERNAL_DEBUGBREAK() (::utl::_private::debug::_throw_runtime_error(), throw)

#else // UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS

#define UTL_INTERNAL_DEBUGBREAK() static_cast<void>(__builtin_trap(), throw)

#ifndef UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS
#define UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS 0
#endif

#endif // UTL_INTERNAL_ASSERT_WITH_EXCEPTIONS

#if UTL(DEBUG_LEVEL) < 2
#define UTL_INTERNAL_INLINE_DEBUG_CALLS UTL_ATTRIB(UTL_ALWAYS_INLINE)
#else // UTL(DEBUG_LEVEL < 2)
#define UTL_INTERNAL_INLINE_DEBUG_CALLS 
#endif // UTL(DEBUG_LEVEL < 2)

//namespace utl {
//	template <typename... Args>
//	[[nodiscard]] std::string format(std::string result, Args&&... args);
//}

namespace utl::_private::debug {

	enum struct assertion_kind {
		assertion, precondition, postcondition
	};

	struct log_data {
		char const* module_name;
		char const* function_name;
		char const* file_name;
		int line;
	};
	
}


namespace utl::_private::debug {
	struct assert_level {};
	struct default_t: assert_level {} inline constexpr _default{};
	struct audit_t: assert_level {} inline constexpr audit{};
	template <typename T>
	inline constexpr bool is_audit = std::is_same_v<T, audit_t>;
	
	
	enum struct log_level {
		trace = 0, info, debug, warning, error, fatal
	};
	inline constexpr auto trace   = log_level::trace;
	inline constexpr auto info    = log_level::info;
	inline constexpr auto debug   = log_level::debug;
	inline constexpr auto warning = log_level::warning;
	inline constexpr auto error   = log_level::error;
	inline constexpr auto fatal   = log_level::fatal;
	
	template <typename T>
	inline constexpr bool is_log_level = std::is_same_v<log_level, T>;
}

namespace utl::_private::debug {
	
	template <typename T>
	auto _is_boolean_testable_impl(int) -> decltype(!std::declval<T>(), std::true_type{});
	template <typename>
	auto _is_boolean_testable_impl(...) -> std::false_type;
	
	template <typename T>
	struct _is_boolean_testable: decltype(_is_boolean_testable_impl<T>(0)) {};
	
	template <typename F>
	concept _boolean_testable_func = _is_boolean_testable<std::invoke_result_t<F>>::value;
	
	template <typename F>
	concept _assert_level = std::is_base_of_v<assert_level, F>;
	
	template <typename F>
	concept _assert_level_func = std::is_base_of_v<assert_level, std::invoke_result_t<F>>;
	
	template <typename F>
	concept _log_level_func = is_log_level<std::invoke_result_t<F>>;
	
	template <typename F>
	concept _message_func = std::is_convertible_v<std::invoke_result_t<F>, char const*>;
	
	template <typename F, typename R, typename ... T>
	concept _invocable_r = std::is_invocable_v<F, T...> && requires(F&& f, T&&... t) {
		static_cast<R>(f(UTL_FORWARD(t)...));
	};
	
	template <typename F, typename... T>
	concept _format_func = _invocable_r<F, fmt::format_string<std::invoke_result_t<T>...>>;
	
	std::false_type report_assertion_failure(assertion_kind, log_data, char const* expr, std::string expanded, char const* message) __attribute__((analyzer_noreturn));
	
	constexpr bool debug_is_active(int debug_level, bool audit) {
		return debug_level > 0 && (!audit || debug_level > 1);
	}
	
	template <int DebugLevel, bool audit>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	constexpr bool _assert_impl(assertion_kind kind, log_data data, auto&& cond, char const* expr, auto&& expanded, char const* message) {
		if constexpr (debug_is_active(DebugLevel, audit)) {
			return cond() || report_assertion_failure(kind, data, expr, expanded(), message);
		}
		else {
			return true;
		}
	}
	
	
	// assert(cond)
	template <int DebugLevel>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	constexpr bool _assert_(assertion_kind kind, log_data data, _boolean_testable_func auto&& cond,
							char const* expr, auto&& expanded) {
		return _assert_impl<DebugLevel, false>(kind, data, UTL_FORWARD(cond), expr, UTL_FORWARD(expanded), nullptr);
	}
	
	// assert(cond, message)
	template <int DebugLevel>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	constexpr bool _assert_(assertion_kind kind,
							log_data data,
							_boolean_testable_func auto&& cond,
							_message_func auto&& message,
							char const* cond_expr,
							char const*,
							auto&& cond_expanded,
							auto&&) {
		return _assert_impl<DebugLevel, false>(kind, data, UTL_FORWARD(cond), cond_expr, UTL_FORWARD(cond_expanded), message());
	}
	
	// assert(level, cond)
	template <int DebugLevel>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	constexpr bool _assert_(assertion_kind kind,
							log_data data,
							_assert_level_func auto&& level,
							_boolean_testable_func auto&& cond,
							char const*,
							char const* cond_expr,
							auto&&,
							auto&& cond_expanded)
	{
		
		return _assert_impl<DebugLevel, is_audit<decltype(level())>>(kind, data, UTL_FORWARD(cond), cond_expr, UTL_FORWARD(cond_expanded), nullptr);
	}

	// assert(level, cond, message)
	template <int DebugLevel>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	constexpr bool _assert_(assertion_kind kind,
							log_data data,
							_assert_level_func auto&& level,
							_boolean_testable_func auto&& cond,
							_message_func auto&& message,
							char const*,
							char const* expr,
							char const*,
							auto&&,
							auto&& expanded,
							auto&&)
	{
		return _assert_impl<DebugLevel, is_audit<decltype(level())>>(kind, data, UTL_FORWARD(cond), expr, UTL_FORWARD(expanded), message());
	}
	
	void _do_log(log_data data, log_level, std::string);
	
	template <int DebugLevel, bool Audit, typename... Args>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	void _log_impl(log_data data,
				   log_level logLevel,
				   fmt::format_string<std::invoke_result_t<Args>...> format, Args&&... args) {
		if constexpr (debug_is_active(DebugLevel, Audit)) {
			_do_log(data, logLevel, utl::format(format, args()...));
		}
	}
	
	template <int DebugLevel, typename... Args>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	void _log_(log_data data,
			   _assert_level_func auto&& assert_level,
			   _log_level_func auto&& log_level,
			   _format_func<Args...> auto format, Args&&... args) {
		_log_impl<DebugLevel, is_audit<decltype(assert_level())>>(data, log_level(), format(), UTL_FORWARD(args)...);
	}
	
	template <int DebugLevel, typename... Args>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	void _log_(log_data data,
			   _log_level_func auto&& log_level,
			   _assert_level_func auto&& assert_level,
			   _format_func<Args...> auto format, Args&&... args) {
		_log_impl<DebugLevel, is_audit<decltype(assert_level())>>(data, log_level(), format(), UTL_FORWARD(args)...);
	}
	
	template <int DebugLevel, typename... Args>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	void _log_(log_data data,
			   _log_level_func auto&& log_level,
			   _format_func<Args...> auto format, Args&&... args) {
		_log_impl<DebugLevel, false>(data, log_level(), format(), UTL_FORWARD(args)...);
	}
	
	template <int DebugLevel, typename... Args>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	void _log_(log_data data,
			   _assert_level_func auto&& assert_level,
			   _format_func<Args...> auto format, Args&&... args) {
		_log_impl<DebugLevel, is_audit<decltype(assert_level())>>(data, log_level::trace, format(), UTL_FORWARD(args)...);
	}
	
	template <int DebugLevel, typename... Args>
	UTL_INTERNAL_INLINE_DEBUG_CALLS
	void _log_(log_data data,
			   _format_func<Args...> auto format, Args&&... args) {
		_log_impl<DebugLevel, false>(data, log_level::trace, format(), UTL_FORWARD(args)...);
	}
	

}




#define UTL_ASSERT_IMPL(module_name, debug_level, kind, ...) \
(\
::utl::_private::debug::_assert_<debug_level>(::utl::_private::debug::assertion_kind::kind, \
											   UTL_INTERNAL_MAKE_LOG_DATA(module_name), \
											   UTL_INTERNAL_DEBUG_WRAP_VALUE_N(__VA_ARGS__), \
											   UTL_INTERNAL_ASSERT_STRINGIFY_VALUE_N(__VA_ARGS__), \
											   UTL_INTERNAL_ASSERT_PARSE_EXPR_N(__VA_ARGS__)) ? (void)0 : (UTL_INTERNAL_DEBUGBREAK()) \
)

#define UTL_INTERNAL_BREAK_IMPL(msg) static_cast<void>(UTL_INTERNAL_DEBUGBREAK())

#if UTL(DEBUG_LEVEL) > 0
#define UTL_INTERNAL_BREAK(msg)   UTL_INTERNAL_BREAK_IMPL(msg)
#else // UTL(DEBUG_LEVEL) > 0
#define UTL_INTERNAL_BREAK(msg)   throw
#endif // UTL(DEBUG_LEVEL) > 0

#include "format.hpp"

#else // UTL(CPP) / C implementation follows

#include <assert.h>
#define UTL_INTERNAL_ASSERT_IMPL(kind, cond, msg) assert(cond)
#define UTL_INTERNAL_BREAK(msg) assert(false)



#endif // UTL(CPP)

//#endif // UTL(DEBUG_LEVEL) > 0

//// these are for forwarding
//#define UTL_INTERNAL_ASSERT_MSG(kind, cond, msg)              UTL_INTERNAL_ASSERT_default(kind, cond, msg)
//#define UTL_INTERNAL_ASSERT_MSG_LEVEL(kind, level, cond, msg) UTL_INTERNAL_ASSERT_##level(kind, cond, msg)
//
////#define UTL_INTERNAL_IGNORE_ASSERT_COND(cond) static_cast<void>(sizeof(cond))
//#define UTL_INTERNAL_IGNORE_ASSERT_COND(cond)

//// choosing the implementation
//#if UTL(DEBUG_LEVEL) > 0
//#define UTL_INTERNAL_ASSERT_default(kind, cond, msg) UTL_INTERNAL_ASSERT_IMPL(kind, cond, msg)
//#else // UTL(DEBUG_LEVEL) > 0
//#define UTL_INTERNAL_ASSERT_default(kind, cond, msg) UTL_INTERNAL_IGNORE_ASSERT_COND(cond)
//#endif // UTL(DEBUG_LEVEL) > 0
//#if UTL(DEBUG_LEVEL) > 1
//#define UTL_INTERNAL_ASSERT_audit(kind, cond, msg)   UTL_INTERNAL_ASSERT_IMPL(kind, cond, msg)
//#else // UTL(DEBUG_LEVEL) > 1
//#define UTL_INTERNAL_ASSERT_audit(kind, cond, msg) UTL_INTERNAL_IGNORE_ASSERT_COND(cond)
//#endif // UTL(DEBUG_LEVEL) > 1



// MARK: static_assert
#if UTL(CPP)
#define UTL_STATIC_ASSERT(...) static_assert(__VA_ARGS__)
#else // UTL(CPP)
#define UTL_STATIC_ASSERT(...)
#endif // UTL(CPP)



//#if UTL(CPP) && UTL(DEBUG_LEVEL) > 0
//#include "strcat.hpp"
//#define utl_log(...) ::utl::_private::debug::log_message(::utl::strcat(__VA_ARGS__).c_str())
//#else
//#define utl_log(...)
//#endif
