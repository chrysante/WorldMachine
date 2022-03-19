#pragma once

#if defined(__cplusplus) && !defined(__METAL_VERSION__)
#	define WORLDMACHINE_CPP
#elif defined(__METAL_VERSION__)
#	define WORLDMACHINE_METAL
#else
#	error Unsupported Language
#endif

#if defined(WORLDMACHINE_CPP)
#	define WM_CONSTANT constexpr
#elif defined(WORLDMACHINE_METAL)
#	define WM_CONSTANT constant
#endif

#if defined(__GNUC__) || defined(__clang__)
#	define WORLDMACHINE_API __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#	define WORLDMACHINE_API __declspec(dllexport)
#endif
