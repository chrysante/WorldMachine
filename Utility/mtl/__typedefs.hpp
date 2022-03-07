/// DELIBERATELY NO INCLUDE GUARDS! Sections have their own include guards.

#include "__base.hpp"
_MTL_SYSTEM_HEADER_

#include "__common.hpp"



	
#ifdef __MTL_DECLARE_STDINT_TYPEDEFS__
#ifndef __MTL_STDINT_TYPEDEFS_INCLUDED__
#define __MTL_STDINT_TYPEDEFS_INCLUDED__
	
#include <cstdint>

namespace _VMTL {
	
	/**
	 These exist so I can conditionally declare 'namespace metal = mtl' in shared header files and use types below as metal::int32_t etc.
	 */

	using std::int8_t;
	using std::int16_t;
	using std::int32_t;
	using std::int64_t;
	
	using std::uint8_t;
	using std::uint16_t;
	using std::uint32_t;
	using std::uint64_t;
	
	using std::size_t;
	using usize = std::size_t;
	using isize = std::ptrdiff_t;
	
}
	
#endif // __MTL_STDINT_TYPEDEFS_INCLUDED__
#endif // __MTL_DECLARE_STDINT_TYPEDEFS__
	
	


namespace _VMTL {
	
	/// MARK: - Complex
#ifdef __MTL_DECLARE_COMPLEX_TYPEDEFS__
#ifndef __MTL_COMPLEX_TYPEDEFS_INCLUDED__
#define __MTL_COMPLEX_TYPEDEFS_INCLUDED__
	
	inline namespace short_types {
		
		using complex_float   = complex<float>;
		using complex_double  = complex<double>;
		using complex_ldouble = complex<long double>;
		
		using complex_int     = complex<int>;
		using complex_long    = complex<long>;
		using complex_llong   = complex<long long>;
		using complex_uint    = complex<unsigned int>;
		using complex_ulong   = complex<unsigned long>;
		using complex_ullong  = complex<unsigned long long>;
		
	}
	
#endif // __MTL_COMPLEX_TYPEDEFS_INCLUDED__
#endif // __MTL_DECLARE_COMPLEX_TYPEDEFS__
	
	/// MARK: - Quaternion
#ifdef __MTL_DECLARE_QUATERNION_TYPEDEFS__
#ifndef __MTL_QUATERNION_TYPEDEFS_INCLUDED__
#define __MTL_QUATERNION_TYPEDEFS_INCLUDED__
	
	inline namespace short_types {
		
		using quaternion_float   = quaternion<float>;
		using quaternion_double  = quaternion<double>;
		using quaternion_ldouble = quaternion<long double>;
		
		using quaternion_int     = quaternion<int>;
		using quaternion_long    = quaternion<long>;
		using quaternion_llong   = quaternion<long long>;
		using quaternion_uint    = quaternion<unsigned int>;
		using quaternion_ulong   = quaternion<unsigned long>;
		using quaternion_ullong  = quaternion<unsigned long long>;
		
	}
	
#endif // __MTL_QUATERNION_TYPEDEFS_INCLUDED__
#endif // __MTL_DECLARE_QUATERNION_TYPEDEFS__
	
	// MARK: - Vector
#ifdef __MTL_DECLARE_VECTOR_TYPEDEFS__
#ifndef __MTL_VECTOR_TYPEDEFS_INCLUDED__
#define __MTL_VECTOR_TYPEDEFS_INCLUDED__
	
	template <typename T, vector_options O = vector_options{}>
	using vector2 = vector<T, 2, O>;
	template <typename T, vector_options O = vector_options{}>
	using vector3 = vector<T, 3, O>;
	template <typename T, vector_options O = vector_options{}>
	using vector4 = vector<T, 4, O>;
	
	inline namespace short_types {
		
		using size2            = vector2<std::size_t>;
		using size3            = vector3<std::size_t>;
		using size4            = vector4<std::size_t>;
		
		using usize2            = vector2<std::size_t>;
		using usize3            = vector3<std::size_t>;
		using usize4            = vector4<std::size_t>;
		
		using isize2            = vector2<std::ptrdiff_t>;
		using isize3            = vector3<std::ptrdiff_t>;
		using isize4            = vector4<std::ptrdiff_t>;
		
		using bool2            = vector2<bool>;
		using bool3            = vector3<bool>;
		using bool4            = vector4<bool>;
		using char2            = vector2<char>;
		using char3            = vector3<char>;
		using char4            = vector4<char>;
		using schar2           = vector2<char>;
		using schar3           = vector3<char>;
		using schar4           = vector4<char>;
		using uchar2           = vector2<unsigned char>;
		using uchar3           = vector3<unsigned char>;
		using uchar4           = vector4<unsigned char>;
		using short2           = vector2<short>;
		using short3           = vector3<short>;
		using short4           = vector4<short>;
		using ushort2          = vector2<unsigned short>;
		using ushort3          = vector3<unsigned short>;
		using ushort4          = vector4<unsigned short>;
		using int2             = vector2<int>;
		using int3             = vector3<int>;
		using int4             = vector4<int>;
		using uint2            = vector2<unsigned int>;
		using uint3            = vector3<unsigned int>;
		using uint4            = vector4<unsigned int>;
		using long2            = vector2<long>;
		using long3            = vector3<long>;
		using long4            = vector4<long>;
		using ulong2           = vector2<unsigned long>;
		using ulong3           = vector3<unsigned long>;
		using ulong4           = vector4<unsigned long>;
		using llong2           = vector2<long long>;
		using llong3           = vector3<long long>;
		using llong4           = vector4<long long>;
		using ullong2          = vector2<unsigned long long>;
		using ullong3          = vector3<unsigned long long>;
		using ullong4          = vector4<unsigned long long>;
		using float2           = vector2<float>;
		using float3           = vector3<float>;
		using float4           = vector4<float>;
		using double2          = vector2<double>;
		using double3          = vector3<double>;
		using double4          = vector4<double>;
		using ldouble2         = vector2<long double>;
		using ldouble3         = vector3<long double>;
		using ldouble4         = vector4<long double>;
		
		using packed_bool2     = vector2<bool,               vector_options{}.packed(true)>;
		using packed_bool3     = vector3<bool,               vector_options{}.packed(true)>;
		using packed_bool4     = vector4<bool,               vector_options{}.packed(true)>;
		using packed_char2     = vector2<char,               vector_options{}.packed(true)>;
		using packed_char3     = vector3<char,               vector_options{}.packed(true)>;
		using packed_char4     = vector4<char,               vector_options{}.packed(true)>;
		using packed_uchar2    = vector2<unsigned char,      vector_options{}.packed(true)>;
		using packed_uchar3    = vector3<unsigned char,      vector_options{}.packed(true)>;
		using packed_uchar4    = vector4<unsigned char,      vector_options{}.packed(true)>;
		using packed_short2    = vector2<short,              vector_options{}.packed(true)>;
		using packed_short3    = vector3<short,              vector_options{}.packed(true)>;
		using packed_short4    = vector4<short,              vector_options{}.packed(true)>;
		using packed_ushort2   = vector2<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort3   = vector3<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort4   = vector4<unsigned short,     vector_options{}.packed(true)>;
		using packed_int2      = vector2<int,                vector_options{}.packed(true)>;
		using packed_int3      = vector3<int,                vector_options{}.packed(true)>;
		using packed_int4      = vector4<int,                vector_options{}.packed(true)>;
		using packed_uint2     = vector2<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint3     = vector3<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint4     = vector4<unsigned int,       vector_options{}.packed(true)>;
		using packed_long2     = vector2<long,               vector_options{}.packed(true)>;
		using packed_long3     = vector3<long,               vector_options{}.packed(true)>;
		using packed_long4     = vector4<long,               vector_options{}.packed(true)>;
		using packed_ulong2    = vector2<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong3    = vector3<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong4    = vector4<unsigned long,      vector_options{}.packed(true)>;
		using packed_llong2    = vector2<long long,          vector_options{}.packed(true)>;
		using packed_llong3    = vector3<long long,          vector_options{}.packed(true)>;
		using packed_llong4    = vector4<long long,          vector_options{}.packed(true)>;
		using packed_ullong2   = vector2<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong3   = vector3<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong4   = vector4<unsigned long long, vector_options{}.packed(true)>;
		using packed_float2    = vector2<float,              vector_options{}.packed(true)>;
		using packed_float3    = vector3<float,              vector_options{}.packed(true)>;
		using packed_float4    = vector4<float,              vector_options{}.packed(true)>;
		using packed_double2   = vector2<double,             vector_options{}.packed(true)>;
		using packed_double3   = vector3<double,             vector_options{}.packed(true)>;
		using packed_double4   = vector4<double,             vector_options{}.packed(true)>;
		using packed_ldouble2  = vector2<long double,        vector_options{}.packed(true)>;
		using packed_ldouble3  = vector3<long double,        vector_options{}.packed(true)>;
		using packed_ldouble4  = vector4<long double,        vector_options{}.packed(true)>;
		
		using aligned_bool2    = vector2<bool,               vector_options{}.packed(false)>;
		using aligned_bool3    = vector3<bool,               vector_options{}.packed(false)>;
		using aligned_bool4    = vector4<bool,               vector_options{}.packed(false)>;
		using aligned_char2    = vector2<char,               vector_options{}.packed(false)>;
		using aligned_char3    = vector3<char,               vector_options{}.packed(false)>;
		using aligned_char4    = vector4<char,               vector_options{}.packed(false)>;
		using aligned_uchar2   = vector2<unsigned char,      vector_options{}.packed(false)>;
		using aligned_uchar3   = vector3<unsigned char,      vector_options{}.packed(false)>;
		using aligned_uchar4   = vector4<unsigned char,      vector_options{}.packed(false)>;
		using aligned_short2   = vector2<short,              vector_options{}.packed(false)>;
		using aligned_short3   = vector3<short,              vector_options{}.packed(false)>;
		using aligned_short4   = vector4<short,              vector_options{}.packed(false)>;
		using aligned_ushort2  = vector2<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort3  = vector3<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort4  = vector4<unsigned short,     vector_options{}.packed(false)>;
		using aligned_int2     = vector2<int,                vector_options{}.packed(false)>;
		using aligned_int3     = vector3<int,                vector_options{}.packed(false)>;
		using aligned_int4     = vector4<int,                vector_options{}.packed(false)>;
		using aligned_uint2    = vector2<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint3    = vector3<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint4    = vector4<unsigned int,       vector_options{}.packed(false)>;
		using aligned_long2    = vector2<long,               vector_options{}.packed(false)>;
		using aligned_long3    = vector3<long,               vector_options{}.packed(false)>;
		using aligned_long4    = vector4<long,               vector_options{}.packed(false)>;
		using aligned_ulong2   = vector2<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong3   = vector3<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong4   = vector4<unsigned long,      vector_options{}.packed(false)>;
		using aligned_llong2   = vector2<long long,          vector_options{}.packed(false)>;
		using aligned_llong3   = vector3<long long,          vector_options{}.packed(false)>;
		using aligned_llong4   = vector4<long long,          vector_options{}.packed(false)>;
		using aligned_ullong2  = vector2<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong3  = vector3<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong4  = vector4<unsigned long long, vector_options{}.packed(false)>;
		using aligned_float2   = vector2<float,              vector_options{}.packed(false)>;
		using aligned_float3   = vector3<float,              vector_options{}.packed(false)>;
		using aligned_float4   = vector4<float,              vector_options{}.packed(false)>;
		using aligned_double2  = vector2<double,             vector_options{}.packed(false)>;
		using aligned_double3  = vector3<double,             vector_options{}.packed(false)>;
		using aligned_double4  = vector4<double,             vector_options{}.packed(false)>;
		using aligned_ldouble2 = vector2<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble3 = vector3<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble4 = vector4<long double,        vector_options{}.packed(false)>;
		
	}
	
#endif // __MTL_VECTOR_TYPEDEFS_INCLUDED__
#endif // __MTL_DECLARE_VECTOR_TYPEDEFS__
	
	/// MARK: - Matrix
#ifdef __MTL_DECLARE_MATRIX_TYPEDEFS__
#ifndef __MTL_MATRIX_TYPEDEFS_INCLUDED__
#define __MTL_MATRIX_TYPEDEFS_INCLUDED__
	
	template <typename T, vector_options O = vector_options{}>
	using matrix2x2 = matrix<T, 2, 2, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix2x3 = matrix<T, 2, 3, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix2x4 = matrix<T, 2, 4, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix3x2 = matrix<T, 3, 2, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix3x3 = matrix<T, 3, 3, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix3x4 = matrix<T, 3, 4, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix4x2 = matrix<T, 4, 2, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix4x3 = matrix<T, 4, 3, O>;
	template <typename T, vector_options O = vector_options{}>
	using matrix4x4 = matrix<T, 4, 4, O>;
	
	inline namespace short_types {
	
		using bool2x2            = matrix2x2<bool,               vector_options{}>;
		using bool2x3            = matrix2x3<bool,               vector_options{}>;
		using bool2x4            = matrix2x4<bool,               vector_options{}>;
		using bool3x2            = matrix3x2<bool,               vector_options{}>;
		using bool3x3            = matrix3x3<bool,               vector_options{}>;
		using bool3x4            = matrix3x4<bool,               vector_options{}>;
		using bool4x2            = matrix4x2<bool,               vector_options{}>;
		using bool4x3            = matrix4x3<bool,               vector_options{}>;
		using bool4x4            = matrix4x4<bool,               vector_options{}>;
		using short2x2           = matrix2x2<short,              vector_options{}>;
		using short2x3           = matrix2x3<short,              vector_options{}>;
		using short2x4           = matrix2x4<short,              vector_options{}>;
		using short3x2           = matrix3x2<short,              vector_options{}>;
		using short3x3           = matrix3x3<short,              vector_options{}>;
		using short3x4           = matrix3x4<short,              vector_options{}>;
		using short4x2           = matrix4x2<short,              vector_options{}>;
		using short4x3           = matrix4x3<short,              vector_options{}>;
		using short4x4           = matrix4x4<short,              vector_options{}>;
		using ushort2x2          = matrix2x2<unsigned short,     vector_options{}>;
		using ushort2x3          = matrix2x3<unsigned short,     vector_options{}>;
		using ushort2x4          = matrix2x4<unsigned short,     vector_options{}>;
		using ushort3x2          = matrix3x2<unsigned short,     vector_options{}>;
		using ushort3x3          = matrix3x3<unsigned short,     vector_options{}>;
		using ushort3x4          = matrix3x4<unsigned short,     vector_options{}>;
		using ushort4x2          = matrix4x2<unsigned short,     vector_options{}>;
		using ushort4x3          = matrix4x3<unsigned short,     vector_options{}>;
		using ushort4x4          = matrix4x4<unsigned short,     vector_options{}>;
		using int2x2             = matrix2x2<int,                vector_options{}>;
		using int2x3             = matrix2x3<int,                vector_options{}>;
		using int2x4             = matrix2x4<int,                vector_options{}>;
		using int3x2             = matrix3x2<int,                vector_options{}>;
		using int3x3             = matrix3x3<int,                vector_options{}>;
		using int3x4             = matrix3x4<int,                vector_options{}>;
		using int4x2             = matrix4x2<int,                vector_options{}>;
		using int4x3             = matrix4x3<int,                vector_options{}>;
		using int4x4             = matrix4x4<int,                vector_options{}>;
		using uint2x2            = matrix2x2<unsigned int,       vector_options{}>;
		using uint2x3            = matrix2x3<unsigned int,       vector_options{}>;
		using uint2x4            = matrix2x4<unsigned int,       vector_options{}>;
		using uint3x2            = matrix3x2<unsigned int,       vector_options{}>;
		using uint3x3            = matrix3x3<unsigned int,       vector_options{}>;
		using uint3x4            = matrix3x4<unsigned int,       vector_options{}>;
		using uint4x2            = matrix4x2<unsigned int,       vector_options{}>;
		using uint4x3            = matrix4x3<unsigned int,       vector_options{}>;
		using uint4x4            = matrix4x4<unsigned int,       vector_options{}>;
		using long2x2            = matrix2x2<long,               vector_options{}>;
		using long2x3            = matrix2x3<long,               vector_options{}>;
		using long2x4            = matrix2x4<long,               vector_options{}>;
		using long3x2            = matrix3x2<long,               vector_options{}>;
		using long3x3            = matrix3x3<long,               vector_options{}>;
		using long3x4            = matrix3x4<long,               vector_options{}>;
		using long4x2            = matrix4x2<long,               vector_options{}>;
		using long4x3            = matrix4x3<long,               vector_options{}>;
		using long4x4            = matrix4x4<long,               vector_options{}>;
		using ulong2x2           = matrix2x2<unsigned long,      vector_options{}>;
		using ulong2x3           = matrix2x3<unsigned long,      vector_options{}>;
		using ulong2x4           = matrix2x4<unsigned long,      vector_options{}>;
		using ulong3x2           = matrix3x2<unsigned long,      vector_options{}>;
		using ulong3x3           = matrix3x3<unsigned long,      vector_options{}>;
		using ulong3x4           = matrix3x4<unsigned long,      vector_options{}>;
		using ulong4x2           = matrix4x2<unsigned long,      vector_options{}>;
		using ulong4x3           = matrix4x3<unsigned long,      vector_options{}>;
		using ulong4x4           = matrix4x4<unsigned long,      vector_options{}>;
		using llong2x2           = matrix2x2<long long,          vector_options{}>;
		using llong2x3           = matrix2x3<long long,          vector_options{}>;
		using llong2x4           = matrix2x4<long long,          vector_options{}>;
		using llong3x2           = matrix3x2<long long,          vector_options{}>;
		using llong3x3           = matrix3x3<long long,          vector_options{}>;
		using llong3x4           = matrix3x4<long long,          vector_options{}>;
		using llong4x2           = matrix4x2<long long,          vector_options{}>;
		using llong4x3           = matrix4x3<long long,          vector_options{}>;
		using llong4x4           = matrix4x4<long long,          vector_options{}>;
		using ullong2x2          = matrix2x2<unsigned long long, vector_options{}>;
		using ullong2x3          = matrix2x3<unsigned long long, vector_options{}>;
		using ullong2x4          = matrix2x4<unsigned long long, vector_options{}>;
		using ullong3x2          = matrix3x2<unsigned long long, vector_options{}>;
		using ullong3x3          = matrix3x3<unsigned long long, vector_options{}>;
		using ullong3x4          = matrix3x4<unsigned long long, vector_options{}>;
		using ullong4x2          = matrix4x2<unsigned long long, vector_options{}>;
		using ullong4x3          = matrix4x3<unsigned long long, vector_options{}>;
		using ullong4x4          = matrix4x4<unsigned long long, vector_options{}>;
		using float2x2           = matrix2x2<float,              vector_options{}>;
		using float2x3           = matrix2x3<float,              vector_options{}>;
		using float2x4           = matrix2x4<float,              vector_options{}>;
		using float3x2           = matrix3x2<float,              vector_options{}>;
		using float3x3           = matrix3x3<float,              vector_options{}>;
		using float3x4           = matrix3x4<float,              vector_options{}>;
		using float4x2           = matrix4x2<float,              vector_options{}>;
		using float4x3           = matrix4x3<float,              vector_options{}>;
		using float4x4           = matrix4x4<float,              vector_options{}>;
		using double2x2          = matrix2x2<double,             vector_options{}>;
		using double2x3          = matrix2x3<double,             vector_options{}>;
		using double2x4          = matrix2x4<double,             vector_options{}>;
		using double3x2          = matrix3x2<double,             vector_options{}>;
		using double3x3          = matrix3x3<double,             vector_options{}>;
		using double3x4          = matrix3x4<double,             vector_options{}>;
		using double4x2          = matrix4x2<double,             vector_options{}>;
		using double4x3          = matrix4x3<double,             vector_options{}>;
		using double4x4          = matrix4x4<double,             vector_options{}>;
		using ldouble2x2         = matrix2x2<long double,        vector_options{}>;
		using ldouble2x3         = matrix2x3<long double,        vector_options{}>;
		using ldouble2x4         = matrix2x4<long double,        vector_options{}>;
		using ldouble3x2         = matrix3x2<long double,        vector_options{}>;
		using ldouble3x3         = matrix3x3<long double,        vector_options{}>;
		using ldouble3x4         = matrix3x4<long double,        vector_options{}>;
		using ldouble4x2         = matrix4x2<long double,        vector_options{}>;
		using ldouble4x3         = matrix4x3<long double,        vector_options{}>;
		using ldouble4x4         = matrix4x4<long double,        vector_options{}>;
	 
		using packed_bool2x2     = matrix2x2<bool,               vector_options{}.packed(true)>;
		using packed_bool2x3     = matrix2x3<bool,               vector_options{}.packed(true)>;
		using packed_bool2x4     = matrix2x4<bool,               vector_options{}.packed(true)>;
		using packed_bool3x2     = matrix3x2<bool,               vector_options{}.packed(true)>;
		using packed_bool3x3     = matrix3x3<bool,               vector_options{}.packed(true)>;
		using packed_bool3x4     = matrix3x4<bool,               vector_options{}.packed(true)>;
		using packed_bool4x2     = matrix4x2<bool,               vector_options{}.packed(true)>;
		using packed_bool4x3     = matrix4x3<bool,               vector_options{}.packed(true)>;
		using packed_bool4x4     = matrix4x4<bool,               vector_options{}.packed(true)>;
		using packed_short2x2    = matrix2x2<short,              vector_options{}.packed(true)>;
		using packed_short2x3    = matrix2x3<short,              vector_options{}.packed(true)>;
		using packed_short2x4    = matrix2x4<short,              vector_options{}.packed(true)>;
		using packed_short3x2    = matrix3x2<short,              vector_options{}.packed(true)>;
		using packed_short3x3    = matrix3x3<short,              vector_options{}.packed(true)>;
		using packed_short3x4    = matrix3x4<short,              vector_options{}.packed(true)>;
		using packed_short4x2    = matrix4x2<short,              vector_options{}.packed(true)>;
		using packed_short4x3    = matrix4x3<short,              vector_options{}.packed(true)>;
		using packed_short4x4    = matrix4x4<short,              vector_options{}.packed(true)>;
		using packed_ushort2x2   = matrix2x2<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort2x3   = matrix2x3<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort2x4   = matrix2x4<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort3x2   = matrix3x2<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort3x3   = matrix3x3<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort3x4   = matrix3x4<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort4x2   = matrix4x2<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort4x3   = matrix4x3<unsigned short,     vector_options{}.packed(true)>;
		using packed_ushort4x4   = matrix4x4<unsigned short,     vector_options{}.packed(true)>;
		using packed_int2x2      = matrix2x2<int,                vector_options{}.packed(true)>;
		using packed_int2x3      = matrix2x3<int,                vector_options{}.packed(true)>;
		using packed_int2x4      = matrix2x4<int,                vector_options{}.packed(true)>;
		using packed_int3x2      = matrix3x2<int,                vector_options{}.packed(true)>;
		using packed_int3x3      = matrix3x3<int,                vector_options{}.packed(true)>;
		using packed_int3x4      = matrix3x4<int,                vector_options{}.packed(true)>;
		using packed_int4x2      = matrix4x2<int,                vector_options{}.packed(true)>;
		using packed_int4x3      = matrix4x3<int,                vector_options{}.packed(true)>;
		using packed_int4x4      = matrix4x4<int,                vector_options{}.packed(true)>;
		using packed_uint2x2     = matrix2x2<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint2x3     = matrix2x3<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint2x4     = matrix2x4<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint3x2     = matrix3x2<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint3x3     = matrix3x3<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint3x4     = matrix3x4<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint4x2     = matrix4x2<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint4x3     = matrix4x3<unsigned int,       vector_options{}.packed(true)>;
		using packed_uint4x4     = matrix4x4<unsigned int,       vector_options{}.packed(true)>;
		using packed_long2x2     = matrix2x2<long,               vector_options{}.packed(true)>;
		using packed_long2x3     = matrix2x3<long,               vector_options{}.packed(true)>;
		using packed_long2x4     = matrix2x4<long,               vector_options{}.packed(true)>;
		using packed_long3x2     = matrix3x2<long,               vector_options{}.packed(true)>;
		using packed_long3x3     = matrix3x3<long,               vector_options{}.packed(true)>;
		using packed_long3x4     = matrix3x4<long,               vector_options{}.packed(true)>;
		using packed_long4x2     = matrix4x2<long,               vector_options{}.packed(true)>;
		using packed_long4x3     = matrix4x3<long,               vector_options{}.packed(true)>;
		using packed_long4x4     = matrix4x4<long,               vector_options{}.packed(true)>;
		using packed_ulong2x2    = matrix2x2<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong2x3    = matrix2x3<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong2x4    = matrix2x4<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong3x2    = matrix3x2<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong3x3    = matrix3x3<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong3x4    = matrix3x4<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong4x2    = matrix4x2<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong4x3    = matrix4x3<unsigned long,      vector_options{}.packed(true)>;
		using packed_ulong4x4    = matrix4x4<unsigned long,      vector_options{}.packed(true)>;
		using packed_llong2x2    = matrix2x2<long long,          vector_options{}.packed(true)>;
		using packed_llong2x3    = matrix2x3<long long,          vector_options{}.packed(true)>;
		using packed_llong2x4    = matrix2x4<long long,          vector_options{}.packed(true)>;
		using packed_llong3x2    = matrix3x2<long long,          vector_options{}.packed(true)>;
		using packed_llong3x3    = matrix3x3<long long,          vector_options{}.packed(true)>;
		using packed_llong3x4    = matrix3x4<long long,          vector_options{}.packed(true)>;
		using packed_llong4x2    = matrix4x2<long long,          vector_options{}.packed(true)>;
		using packed_llong4x3    = matrix4x3<long long,          vector_options{}.packed(true)>;
		using packed_llong4x4    = matrix4x4<long long,          vector_options{}.packed(true)>;
		using packed_ullong2x2   = matrix2x2<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong2x3   = matrix2x3<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong2x4   = matrix2x4<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong3x2   = matrix3x2<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong3x3   = matrix3x3<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong3x4   = matrix3x4<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong4x2   = matrix4x2<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong4x3   = matrix4x3<unsigned long long, vector_options{}.packed(true)>;
		using packed_ullong4x4   = matrix4x4<unsigned long long, vector_options{}.packed(true)>;
		using packed_float2x2    = matrix2x2<float,              vector_options{}.packed(true)>;
		using packed_float2x3    = matrix2x3<float,              vector_options{}.packed(true)>;
		using packed_float2x4    = matrix2x4<float,              vector_options{}.packed(true)>;
		using packed_float3x2    = matrix3x2<float,              vector_options{}.packed(true)>;
		using packed_float3x3    = matrix3x3<float,              vector_options{}.packed(true)>;
		using packed_float3x4    = matrix3x4<float,              vector_options{}.packed(true)>;
		using packed_float4x2    = matrix4x2<float,              vector_options{}.packed(true)>;
		using packed_float4x3    = matrix4x3<float,              vector_options{}.packed(true)>;
		using packed_float4x4    = matrix4x4<float,              vector_options{}.packed(true)>;
		using packed_double2x2   = matrix2x2<double,             vector_options{}.packed(true)>;
		using packed_double2x3   = matrix2x3<double,             vector_options{}.packed(true)>;
		using packed_double2x4   = matrix2x4<double,             vector_options{}.packed(true)>;
		using packed_double3x2   = matrix3x2<double,             vector_options{}.packed(true)>;
		using packed_double3x3   = matrix3x3<double,             vector_options{}.packed(true)>;
		using packed_double3x4   = matrix3x4<double,             vector_options{}.packed(true)>;
		using packed_double4x2   = matrix4x2<double,             vector_options{}.packed(true)>;
		using packed_double4x3   = matrix4x3<double,             vector_options{}.packed(true)>;
		using packed_double4x4   = matrix4x4<double,             vector_options{}.packed(true)>;
		using packed_ldouble2x2  = matrix2x2<long double,        vector_options{}.packed(true)>;
		using packed_ldouble2x3  = matrix2x3<long double,        vector_options{}.packed(true)>;
		using packed_ldouble2x4  = matrix2x4<long double,        vector_options{}.packed(true)>;
		using packed_ldouble3x2  = matrix3x2<long double,        vector_options{}.packed(true)>;
		using packed_ldouble3x3  = matrix3x3<long double,        vector_options{}.packed(true)>;
		using packed_ldouble3x4  = matrix3x4<long double,        vector_options{}.packed(true)>;
		using packed_ldouble4x2  = matrix4x2<long double,        vector_options{}.packed(true)>;
		using packed_ldouble4x3  = matrix4x3<long double,        vector_options{}.packed(true)>;
		using packed_ldouble4x4  = matrix4x4<long double,        vector_options{}.packed(true)>;
		
		using aligned_bool2x2    = matrix2x2<bool,               vector_options{}.packed(false)>;
		using aligned_bool2x3    = matrix2x3<bool,               vector_options{}.packed(false)>;
		using aligned_bool2x4    = matrix2x4<bool,               vector_options{}.packed(false)>;
		using aligned_bool3x2    = matrix3x2<bool,               vector_options{}.packed(false)>;
		using aligned_bool3x3    = matrix3x3<bool,               vector_options{}.packed(false)>;
		using aligned_bool3x4    = matrix3x4<bool,               vector_options{}.packed(false)>;
		using aligned_bool4x2    = matrix4x2<bool,               vector_options{}.packed(false)>;
		using aligned_bool4x3    = matrix4x3<bool,               vector_options{}.packed(false)>;
		using aligned_bool4x4    = matrix4x4<bool,               vector_options{}.packed(false)>;
		using aligned_short2x2   = matrix2x2<short,              vector_options{}.packed(false)>;
		using aligned_short2x3   = matrix2x3<short,              vector_options{}.packed(false)>;
		using aligned_short2x4   = matrix2x4<short,              vector_options{}.packed(false)>;
		using aligned_short3x2   = matrix3x2<short,              vector_options{}.packed(false)>;
		using aligned_short3x3   = matrix3x3<short,              vector_options{}.packed(false)>;
		using aligned_short3x4   = matrix3x4<short,              vector_options{}.packed(false)>;
		using aligned_short4x2   = matrix4x2<short,              vector_options{}.packed(false)>;
		using aligned_short4x3   = matrix4x3<short,              vector_options{}.packed(false)>;
		using aligned_short4x4   = matrix4x4<short,              vector_options{}.packed(false)>;
		using aligned_ushort2x2  = matrix2x2<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort2x3  = matrix2x3<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort2x4  = matrix2x4<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort3x2  = matrix3x2<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort3x3  = matrix3x3<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort3x4  = matrix3x4<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort4x2  = matrix4x2<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort4x3  = matrix4x3<unsigned short,     vector_options{}.packed(false)>;
		using aligned_ushort4x4  = matrix4x4<unsigned short,     vector_options{}.packed(false)>;
		using aligned_int2x2     = matrix2x2<int,                vector_options{}.packed(false)>;
		using aligned_int2x3     = matrix2x3<int,                vector_options{}.packed(false)>;
		using aligned_int2x4     = matrix2x4<int,                vector_options{}.packed(false)>;
		using aligned_int3x2     = matrix3x2<int,                vector_options{}.packed(false)>;
		using aligned_int3x3     = matrix3x3<int,                vector_options{}.packed(false)>;
		using aligned_int3x4     = matrix3x4<int,                vector_options{}.packed(false)>;
		using aligned_int4x2     = matrix4x2<int,                vector_options{}.packed(false)>;
		using aligned_int4x3     = matrix4x3<int,                vector_options{}.packed(false)>;
		using aligned_int4x4     = matrix4x4<int,                vector_options{}.packed(false)>;
		using aligned_uint2x2    = matrix2x2<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint2x3    = matrix2x3<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint2x4    = matrix2x4<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint3x2    = matrix3x2<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint3x3    = matrix3x3<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint3x4    = matrix3x4<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint4x2    = matrix4x2<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint4x3    = matrix4x3<unsigned int,       vector_options{}.packed(false)>;
		using aligned_uint4x4    = matrix4x4<unsigned int,       vector_options{}.packed(false)>;
		using aligned_long2x2    = matrix2x2<long,               vector_options{}.packed(false)>;
		using aligned_long2x3    = matrix2x3<long,               vector_options{}.packed(false)>;
		using aligned_long2x4    = matrix2x4<long,               vector_options{}.packed(false)>;
		using aligned_long3x2    = matrix3x2<long,               vector_options{}.packed(false)>;
		using aligned_long3x3    = matrix3x3<long,               vector_options{}.packed(false)>;
		using aligned_long3x4    = matrix3x4<long,               vector_options{}.packed(false)>;
		using aligned_long4x2    = matrix4x2<long,               vector_options{}.packed(false)>;
		using aligned_long4x3    = matrix4x3<long,               vector_options{}.packed(false)>;
		using aligned_long4x4    = matrix4x4<long,               vector_options{}.packed(false)>;
		using aligned_ulong2x2   = matrix2x2<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong2x3   = matrix2x3<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong2x4   = matrix2x4<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong3x2   = matrix3x2<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong3x3   = matrix3x3<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong3x4   = matrix3x4<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong4x2   = matrix4x2<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong4x3   = matrix4x3<unsigned long,      vector_options{}.packed(false)>;
		using aligned_ulong4x4   = matrix4x4<unsigned long,      vector_options{}.packed(false)>;
		using aligned_llong2x2   = matrix2x2<long long,          vector_options{}.packed(false)>;
		using aligned_llong2x3   = matrix2x3<long long,          vector_options{}.packed(false)>;
		using aligned_llong2x4   = matrix2x4<long long,          vector_options{}.packed(false)>;
		using aligned_llong3x2   = matrix3x2<long long,          vector_options{}.packed(false)>;
		using aligned_llong3x3   = matrix3x3<long long,          vector_options{}.packed(false)>;
		using aligned_llong3x4   = matrix3x4<long long,          vector_options{}.packed(false)>;
		using aligned_llong4x2   = matrix4x2<long long,          vector_options{}.packed(false)>;
		using aligned_llong4x3   = matrix4x3<long long,          vector_options{}.packed(false)>;
		using aligned_llong4x4   = matrix4x4<long long,          vector_options{}.packed(false)>;
		using aligned_ullong2x2  = matrix2x2<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong2x3  = matrix2x3<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong2x4  = matrix2x4<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong3x2  = matrix3x2<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong3x3  = matrix3x3<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong3x4  = matrix3x4<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong4x2  = matrix4x2<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong4x3  = matrix4x3<unsigned long long, vector_options{}.packed(false)>;
		using aligned_ullong4x4  = matrix4x4<unsigned long long, vector_options{}.packed(false)>;
		using aligned_float2x2   = matrix2x2<float,              vector_options{}.packed(false)>;
		using aligned_float2x3   = matrix2x3<float,              vector_options{}.packed(false)>;
		using aligned_float2x4   = matrix2x4<float,              vector_options{}.packed(false)>;
		using aligned_float3x2   = matrix3x2<float,              vector_options{}.packed(false)>;
		using aligned_float3x3   = matrix3x3<float,              vector_options{}.packed(false)>;
		using aligned_float3x4   = matrix3x4<float,              vector_options{}.packed(false)>;
		using aligned_float4x2   = matrix4x2<float,              vector_options{}.packed(false)>;
		using aligned_float4x3   = matrix4x3<float,              vector_options{}.packed(false)>;
		using aligned_float4x4   = matrix4x4<float,              vector_options{}.packed(false)>;
		using aligned_double2x2  = matrix2x2<double,             vector_options{}.packed(false)>;
		using aligned_double2x3  = matrix2x3<double,             vector_options{}.packed(false)>;
		using aligned_double2x4  = matrix2x4<double,             vector_options{}.packed(false)>;
		using aligned_double3x2  = matrix3x2<double,             vector_options{}.packed(false)>;
		using aligned_double3x3  = matrix3x3<double,             vector_options{}.packed(false)>;
		using aligned_double3x4  = matrix3x4<double,             vector_options{}.packed(false)>;
		using aligned_double4x2  = matrix4x2<double,             vector_options{}.packed(false)>;
		using aligned_double4x3  = matrix4x3<double,             vector_options{}.packed(false)>;
		using aligned_double4x4  = matrix4x4<double,             vector_options{}.packed(false)>;
		using aligned_ldouble2x2 = matrix2x2<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble2x3 = matrix2x3<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble2x4 = matrix2x4<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble3x2 = matrix3x2<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble3x3 = matrix3x3<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble3x4 = matrix3x4<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble4x2 = matrix4x2<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble4x3 = matrix4x3<long double,        vector_options{}.packed(false)>;
		using aligned_ldouble4x4 = matrix4x4<long double,        vector_options{}.packed(false)>;
		
	}
	
#endif // __MTL_MATRIX_TYPEDEFS_INCLUDED__
#endif // __MTL_DECLARE_MATRIX_TYPEDEFS__
	
}
