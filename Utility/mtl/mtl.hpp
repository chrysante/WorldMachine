#pragma once

#ifndef __MTL_MTL_HPP_INCLUDED__
#define __MTL_MTL_HPP_INCLUDED__

#include "core.hpp"
#include "ext.hpp"

#endif // __MTL_MTL_HPP_INCLUDED__



/// MARK: Memory Layout
/// By default memory layout is 'Aligned', implying the following alignments:
//  Data Type       | Alignment
//  vector<T, 2>    | 2 * alignof(T)
//  vector<T, 3>    | 4 * alignof(T)
//  vector<T, 4>    | 4 * alignof(T)
//  matrix<T, N, 2> | 2 * alignof(T) [where N = 2, 3, 4]
//  matrix<T, N, 3> | 4 * alignof(T) [where N = 2, 3, 4]
//  matrix<T, N, 4> | 4 * alignof(T) [where N = 2, 3, 4]
///
/// With memory layout 'Packed' every data type is aligned as it's underlying fundamental type,
/// e.g. alignof(float3) = 4, alignof(vector4<int64_t>) = 8


/// MARK: - Customization Points
// +------------------------------+-----------------+--------+
// | Macro                        | Possible Values | Default|
// +------------------------------+-----------------+--------+
// | MTL_DEBUG_LEVEL              |              0  |       0|  On Debug Level 0 all assertions and API validations
// |                              |                 |        |  are disabled.
// |                              |                 |        |  Most (small) functions are forced inline.
// |                              |                 |        |  No debug symbols are generated for public API.
// |                              |                 |        |
// |                              |              1  |        |  On Debug Level 1 API validations are enabled, including
// |                              |                 |        |  bounds checks in operator[].
// |                              |                 |        |  Most (small) functions are forced inline.
// |                              |                 |        |  No debug symbols are generated for public API.
// |                              |                 |        |
// |                              |              2  |        |  On Debug Level 2 internel assertions and costly checks
// |                              |                 |        |  also are enabled.
// |                              |                 |        |  No functions are forced inline.
// |                              |                 |        |  Debug symbols are generated all functions.
// |                              |                 |        |
// +------------------------------+-----------------+--------+
// | MTL_SAFE_MATH                |              0, |       1|  If disabled some math operations like exp(complex),
// |                              |              1  |        |  sin(complex) and norm(vector) don't handle overflow
// |                              |                 |        |  correctly. Disabling this might speed up these
// |                              |                 |        |  operations.
// +------------------------------+-----------------+--------+
// | MTL_DEFAULT_PACKED           |              0, |       0|  If enabled vectors and matrices use 'Packed' memory
// |                              |              1  |        |  layout by default. Otherwise memory layout is 'Aligned'.
// |                              |                 |        |  Regardless of what is specified here, the typedefs
// |                              |                 |        |  aligned_*** and packed_*** are defined and the alignment
// |                              |                 |        |  can also be specified in the vector_options template
// |                              |                 |        |  parameter.
// +------------------------------+-----------------+--------+
// | MTL_NAMESPACE_NAME           |              -  |     mtl|  Change the name of the 'mtl' namespace. Can be useful
// |                              |                 |        |  to share code between C++ and shader header files.
// +------------------------------+-----------------+--------+
// | MTL_UNICODE_MATH_PARANTHESES |              0, |       1|
// |                              |              1  |        |
// +------------------------------+-----------------+--------+
