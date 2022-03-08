#pragma once

#ifndef __UTL_LOG_HPP_INCLUDED__
#define __UTL_LOG_HPP_INCLUDED__

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "format.hpp"
#include "vector.hpp"
#include "functional.hpp"
#include "operator_definition_macros.hpp"
#include "static_string.hpp"

#include <chrono>
#include <thread>
#include <string>
#include <ostream>
#include <array>


namespace utl {
	
	enum struct log_level {
		trace   =  1 << 0,
		info    =  1 << 1,
		debug   =  1 << 2,
		warning =  1 << 3,
		error   =  1 << 4,
		fatal   =  1 << 5,
		all     = (1 << 6) - 1
	};
	UTL_ENUM_BITWISE_OPERATORS_DEF(log_level);
	
//	inline std::ostream& operator<<(std::ostream& str, log_level level) {
//		constexpr auto names = std::array{
//			"Trace",
//			"Info",
//			"Debug",
//			"Warning",
//			"Error",
//			"Fatal"
//		};
//		return str << names[utl::to_underlying(level)];
//	}
	
	struct source_info {
		char const* function;
		char const* file;
		int line;
	};
	
	struct log_message {
		std::string message;
		log_level level;
		
		std::chrono::time_point<std::chrono::system_clock> time;
		std::thread::id thread_id;
		std::string_view logger_name;
		
		source_info source_info;
	};
	
	class logger {
	public:
		logger(std::string name): _name(std::move(name)) {}
		
		template <std::size_t N>
		void log(source_info source_info, log_level mask, log_level level, utl::basic_static_string<N> const& format_string, auto&&... args) {
			if (!test(mask & level)) { return; }
			do_log(log_message{
				.message     = utl::format(format_string.data(), UTL_FORWARD(args)...),
				.level       = level,
				.time        = std::chrono::system_clock::now(),
				.thread_id   = std::this_thread::get_id(),
				.logger_name = _name,
				.source_info = source_info
			});
		}
		
		template <std::size_t N>
		void log(source_info source_info, log_level mask, log_level level, char const (&format_string)[N], auto&&... args) {
			return log(source_info, mask, level, utl::basic_static_string(format_string), UTL_FORWARD(args)...);
		}
		
		template <std::size_t N>
		void log(source_info source_info, log_level mask, utl::basic_static_string<N> const& format_string, auto&&... args) {
			return log(source_info, mask, log_level::trace, format_string, UTL_FORWARD(args)...);
		}
		
		void add_listener(utl::function<void(log_message)> listener) {
			_listeners.push_back(std::move(listener));
		}
		
	private:
		void do_log(log_message);
		
	private:
		utl::vector<utl::function<void(log_message)>> _listeners;
		std::string _name;
	};
	
	utl::function<void(log_message)> make_stdout_listener();
	
	std::string time_in_HH_MM_SS_MMM(std::chrono::time_point<std::chrono::system_clock>);
	
}

#define UTL_MAKE_SOURCE_INFO() \
::utl::source_info { __PRETTY_FUNCTION__, __FILE__, __LINE__ }

#define UTL_MAKE_LOG_MACRO(Logger, LogLevelMask, FirstArg, ...) \
	Logger.log(UTL_MAKE_SOURCE_INFO(), LogLevelMask,            \
			   UTL_INTERNAL_WRAP_LOG_STATEMENT_USING_DECL(FirstArg) __VA_OPT__(,) __VA_ARGS__)



namespace utl {
	struct __log_passthrough_t {
		constexpr utl::log_level operator()(utl::log_level l) const {
			return l;
		}
		template <std::size_t N>
		constexpr auto operator()(char const (&fmt)[N]) const { return utl::basic_static_string(fmt); }
	} constexpr __log_passthrough{};
}


#define UTL_INTERNAL_WRAP_LOG_STATEMENT_USING_DECL(...) [&]() {                  \
	[[maybe_unused]] constexpr utl::log_level trace   = utl::log_level::trace;   \
	[[maybe_unused]] constexpr utl::log_level info    = utl::log_level::info;    \
	[[maybe_unused]] constexpr utl::log_level debug   = utl::log_level::debug;   \
	[[maybe_unused]] constexpr utl::log_level warning = utl::log_level::warning; \
	[[maybe_unused]] constexpr utl::log_level error   = utl::log_level::error;   \
	[[maybe_unused]] constexpr utl::log_level fatal   = utl::log_level::fatal;   \
	return utl::__log_passthrough(__VA_ARGS__);                                  \
}()

#endif // __UTL_LOG_HPP_INCLUDED__
