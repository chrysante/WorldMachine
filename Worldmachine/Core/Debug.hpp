#pragma once

#include "Core/Base.hpp"

#if WORLDMACHINE(CPP)

#include <utl/debug.hpp>
#include <utl/log.hpp>
#include <mutex>

#ifndef WM_DEBUGLEVEL
#define WM_DEBUGLEVEL 0
#endif

#define WM_Assert(...) \
	UTL_ASSERT_IMPL("WorldMachine", WM_DEBUGLEVEL, assertion, __VA_ARGS__)
#define WM_Expect(...) \
	UTL_ASSERT_IMPL("WorldMachine", WM_DEBUGLEVEL, precondition, __VA_ARGS__)
#define WM_Ensure(...) \
	UTL_ASSERT_IMPL("WorldMachine", WM_DEBUGLEVEL, postcondition, __VA_ARGS__)
#define WM_DebugBreak(msg) \
	utl_debugbreak(msg)
#define WM_BoundsCheck(index, lower, upper) \
	UTL_BOUNDS_CHECK_IMPL("WorldMachine", WM_DEBUGLEVEL, index, lower, upper)

namespace worldmachine {
	constexpr utl::log_level logLevelMask = WM_LOGLEVEL == 0 ? utl::log_level::warning | utl::log_level::error | utl::log_level::fatal : utl::log_level::all;
	std::pair<std::unique_lock<std::mutex>, utl::vector<utl::log_message>&> globalLog();
	utl::logger& globalLogger();
}

#define WM_Log(...)                                    \
	UTL_MAKE_LOG_MACRO(::worldmachine::globalLogger(), \
					   ::worldmachine::logLevelMask,   \
					   __VA_ARGS__)

#endif // WORLDMACHINE(CPP)
