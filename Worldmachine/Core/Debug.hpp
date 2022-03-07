#pragma once

#include "Core/Base.hpp"

#if WORLDMACHINE(CPP)

#include <utl/debug.hpp>

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

#ifndef WM_LOGLEVEL
#define WM_LOGLEVEL 0
#endif

#define WM_Log(...) UTL_LOG_IMPL("WorldMachine", WM_LOGLEVEL, __VA_ARGS__)

#endif // WORLDMACHINE(CPP)
