#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#if UTL(CPP)
#define UTL_INTERNAL_STL_NAMESPACE std
#else
#define UTL_INTERNAL_STL_NAMESPACE metal
#endif

#define UTL_INTERNAL_ENUM_UN_PREF_OPERATOR_DEF(TYPE, RESULT_TYPE, OP) \
inline constexpr RESULT_TYPE operator OP(TYPE a) { \
	return static_cast<RESULT_TYPE>(OP static_cast<typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type>(a)); \
}
#define UTL_INTERNAL_ENUM_BIN_OPERATOR_DEF(TYPE, OP) \
inline constexpr TYPE operator OP(TYPE a, TYPE b) { \
	return static_cast<TYPE>(static_cast<typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type>(a) OP static_cast<typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type>(b)); \
} \
inline constexpr TYPE operator OP(typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type a, TYPE b) { \
	return static_cast<TYPE>(a OP static_cast<typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type>(b)); \
} \
inline constexpr TYPE operator OP(TYPE a, typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type b) { \
	return static_cast<TYPE>(static_cast<typename UTL_INTERNAL_STL_NAMESPACE::underlying_type<TYPE>::type>(a) OP b); \
}

#if UTL(CPP)
#define UTL_INTERNAL_ENUM_BIN_ASSIGNMENT_DEF(TYPE, OP) \
inline constexpr TYPE& operator OP##=(TYPE& a, TYPE b) { \
	return a = a OP b; \
}
#else
#define UTL_INTERNAL_ENUM_BIN_ASSIGNMENT_DEF(TYPE, OP)
#endif

#define UTL_INTERNAL_ENUM_BITWISE_OPERATORS_DEF(TYPE) \
UTL_INTERNAL_ENUM_BIN_OPERATOR_DEF(TYPE, &) \
UTL_INTERNAL_ENUM_BIN_OPERATOR_DEF(TYPE, |) \
UTL_INTERNAL_ENUM_BIN_OPERATOR_DEF(TYPE, ^) \
UTL_INTERNAL_ENUM_BIN_ASSIGNMENT_DEF(TYPE, &) \
UTL_INTERNAL_ENUM_BIN_ASSIGNMENT_DEF(TYPE, |) \
UTL_INTERNAL_ENUM_BIN_ASSIGNMENT_DEF(TYPE, ^) \
UTL_INTERNAL_ENUM_UN_PREF_OPERATOR_DEF(TYPE, TYPE, ~) \
UTL_INTERNAL_ENUM_UN_PREF_OPERATOR_DEF(TYPE, bool, !)

#define UTL_ENUM_BITWISE_OPERATORS_DEF(...) \
UTL_INTERNAL_ENUM_BITWISE_OPERATORS_DEF(__VA_ARGS__) \
constexpr bool test(__VA_ARGS__ value) { return !!value; }
