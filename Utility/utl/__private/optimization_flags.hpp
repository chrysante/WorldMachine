#pragma once

// included by base
UTL(SYSTEM_HEADER)

#define UTL_LIKELY(x)   (__builtin_expect((x), 1))
#define UTL_UNLIKELY(x) (__builtin_expect((x), 0))

#ifndef UTL_ATTRIB
#define UTL_ATTRIB(x) __attribute__((UTL_##x))
#endif

#define UTL_PURE  pure
#define UTL_CONST const

#if UTL(DEBUG_LEVEL) < 2
#define UTL_ALWAYS_INLINE always_inline
#define UTL_NODEBUG nodebug
#else
#define UTL_ALWAYS_INLINE
#define UTL_NODEBUG
#endif


#define UTL_NOINLINE noinline

