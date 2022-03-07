#pragma once


vec<T, 2, math_internal::make_swizzle_options(O, 1, 1)> xx;
vec<T, 2, math_internal::make_swizzle_options(O, 1, 2)> xy;
vec<T, 2, math_internal::make_swizzle_options(O, 2, 1)> yx;
vec<T, 2, math_internal::make_swizzle_options(O, 2, 2)> yy;

vec<T, 3, math_internal::make_swizzle_options(O, 1, 1, 1)> xxx;
vec<T, 3, math_internal::make_swizzle_options(O, 1, 1, 2)> xxy;
vec<T, 3, math_internal::make_swizzle_options(O, 1, 2, 1)> xyx;
vec<T, 3, math_internal::make_swizzle_options(O, 1, 2, 2)> xyy;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 1, 1)> yxx;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 1, 2)> yxy;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 2, 1)> yyx;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 2, 2)> yyy;

vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 1, 1)> xxxx;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 1, 2)> xxxy;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 2, 1)> xxyx;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 2, 2)> xxyy;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 1, 1)> xyxx;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 1, 2)> xyxy;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 2, 1)> xyyx;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 2, 2)> xyyy;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 1, 1)> yxxx;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 1, 2)> yxxy;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 2, 1)> yxyx;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 2, 2)> yxyy;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 1, 1)> yyxx;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 1, 2)> yyxy;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 2, 1)> yyyx;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 2, 2)> yyyy;

vec<T, 2, math_internal::make_swizzle_options(O, 1, 1)> rr;
vec<T, 2, math_internal::make_swizzle_options(O, 1, 2)> rg;
vec<T, 2, math_internal::make_swizzle_options(O, 2, 1)> gr;
vec<T, 2, math_internal::make_swizzle_options(O, 2, 2)> gg;

vec<T, 3, math_internal::make_swizzle_options(O, 1, 1, 1)> rrr;
vec<T, 3, math_internal::make_swizzle_options(O, 1, 1, 2)> rrg;
vec<T, 3, math_internal::make_swizzle_options(O, 1, 2, 1)> rgr;
vec<T, 3, math_internal::make_swizzle_options(O, 1, 2, 2)> rgg;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 1, 1)> grr;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 1, 2)> grg;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 2, 1)> ggr;
vec<T, 3, math_internal::make_swizzle_options(O, 2, 2, 2)> ggg;

vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 1, 1)> rrrr;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 1, 2)> rrrg;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 2, 1)> rrgr;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 1, 2, 2)> rrgg;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 1, 1)> rgrr;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 1, 2)> rgrg;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 2, 1)> rggr;
vec<T, 4, math_internal::make_swizzle_options(O, 1, 2, 2, 2)> rggg;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 1, 1)> grrr;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 1, 2)> grrg;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 2, 1)> grgr;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 1, 2, 2)> grgg;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 1, 1)> ggrr;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 1, 2)> ggrg;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 2, 1)> gggr;
vec<T, 4, math_internal::make_swizzle_options(O, 2, 2, 2, 2)> gggg;

