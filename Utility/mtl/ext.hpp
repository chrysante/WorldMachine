#pragma once

#ifndef __MTL_EXT_HPP_INCLUDED__
#define __MTL_EXT_HPP_INCLUDED__

#include "__base.hpp"
_MTL_SYSTEM_HEADER_


#include "__common.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include <cmath>

namespace _VMTL {

	/// MARK: - 4x4 Matrix Factories
	/// ortho() -> Orthogonal Projection Matrix
	template <handedness H = default_handedness, real_scalar T, vector_options O = vector_options{}>
	__mtl_mathfunction __mtl_interface_export
	constexpr matrix4x4<T, O> ortho(T const& left,
									T const& right,
									T const& bottom,
									T const& top,
									T const& zNear,
									T const& zFar)
	{
		if constexpr (H == left_handed) {
			return {
				T(2) / (right - left), 0,                     0,                     -(right + left) / (right - left),
				0,                     T(2) / (top - bottom), 0,                     -(top + bottom) / (top - bottom),
				0,                     0,                     T(1) / (zFar - zNear), -zNear / (zFar - zNear),
				0,                     0,                     0,                      1
			};
		}
		else {
			static_assert(H == right_handed);
			matrix4x4<T, O> Result(1);
			Result.__at(0, 0) =   T(2) / (right - left);
			Result.__at(1, 1) =   T(2) / (top - bottom);
			Result.__at(2, 2) = - T(1) / (zFar - zNear);
			Result.__at(0, 3) = - (right + left) / (right - left);
			Result.__at(1, 3) = - (top + bottom) / (top - bottom);
			Result.__at(2, 3) = - zNear / (zFar - zNear);
			return Result;
		}
	}
	
	/// look_at() -> View Matrix
	template <handedness H = default_handedness,
			  typename T, vector_options O,
			  typename U, vector_options P,
			  typename V, vector_options Q>
	__mtl_mathfunction __mtl_interface_export
	constexpr matrix4x4<__mtl_promote(T, U, V), combine(O, P, Q)>
	look_at(vector3<T, O> const& eye,
			vector3<U, P> const& center,
			vector3<V, Q> const& up)
	{
		using TT = __mtl_promote(T, U, V);
		auto constexpr OO = combine(O, P, Q);
		
		if constexpr (H == left_handed) {
			vector3<TT, OO> const f = normalize(center - eye);
			vector3<TT, OO> const s = normalize(cross(up, f));
			vector3<TT, OO> const u = cross(f, s);

			matrix4x4<TT, OO> Result(1);
			Result.__at(0, 0) = s.__at(0);
			Result.__at(0, 1) = s.__at(1);
			Result.__at(0, 2) = s.__at(2);
			Result.__at(1, 0) = u.__at(0);
			Result.__at(1, 1) = u.__at(1);
			Result.__at(1, 2) = u.__at(2);
			Result.__at(2, 0) = f.__at(0);
			Result.__at(2, 1) = f.__at(1);
			Result.__at(2, 2) = f.__at(2);
			Result.__at(0, 3) = -dot(s, eye);
			Result.__at(1, 3) = -dot(u, eye);
			Result.__at(2, 3) = -dot(f, eye);
			return Result;
		}
		else {
			static_assert(H == right_handed);
			vector3<TT, OO> const f = normalize(center - eye);
			vector3<TT, OO> const s = normalize(cross(f, up));
			vector3<TT, OO> const u = cross(s, f);

			matrix4x4<TT, OO> Result(1);
			Result.__at(0, 0) =  s.__at(0);
			Result.__at(0, 1) =  s.__at(1);
			Result.__at(0, 2) =  s.__at(2);
			Result.__at(1, 0) =  u.__at(0);
			Result.__at(1, 1) =  u.__at(1);
			Result.__at(1, 2) =  u.__at(2);
			Result.__at(2, 0) = -f.__at(0);
			Result.__at(2, 1) = -f.__at(1);
			Result.__at(2, 2) = -f.__at(2);
			Result.__at(0, 3) = -dot(s, eye);
			Result.__at(1, 3) = -dot(u, eye);
			Result.__at(2, 3) =  dot(f, eye);
			return Result;
		}
	}
	
	template <handedness H = default_handedness, vector_options O = vector_options{}>
	__mtl_mathfunction __mtl_interface_export
	constexpr auto ortho(real_scalar auto const& left,
						 real_scalar auto const& right,
						 real_scalar auto const& bottom,
						 real_scalar auto const& top,
						 real_scalar auto const& zNear,
						 real_scalar auto const& zFar)
	{
		using T = __mtl_promote(__mtl_decltype_stripped(left),
								__mtl_decltype_stripped(right),
								__mtl_decltype_stripped(bottom),
								__mtl_decltype_stripped(top),
								__mtl_decltype_stripped(zNear),
								__mtl_decltype_stripped(zFar));
		return ortho<H, T>(left, right, bottom, top, zNear, zFar);
	}
	
	template <handedness H = default_handedness, vector_options O = vector_options{}>
	__mtl_mathfunction __mtl_interface_export
	constexpr auto infinite_perspective(real_scalar auto const& field_of_view,
										real_scalar auto const& aspect_ratio,
										real_scalar auto const& near)
	{
		using T = __mtl_promote(__mtl_decltype_stripped(field_of_view),
								__mtl_decltype_stripped(aspect_ratio),
								__mtl_decltype_stripped(near));
		T const range = std::tan(field_of_view / T(2)) * near;
		T const left = -range * aspect_ratio;
		T const right = range * aspect_ratio;
		T const bottom = -range;
		T const top = range;
		if constexpr (H == left_handed) {
			matrix<T, 4, 4, O> result{};
			result(0, 0) = (T(2) * near) / (right - left);
			result(1, 1) = (T(2) * near) / (top - bottom);
			result(2, 2) =  T(1);
			result(3, 2) =  T(1);
			result(2, 3) = -T(2) * near;
			return result;
		}
		else {
			matrix<T, 4, 4, O> result{};
			result(0, 0) = (T(2) * near) / (right - left);
			result(1, 1) = (T(2) * near) / (top - bottom);
			result(2, 2) = -T(1);
			result(3, 2) = -T(1);
			result(2, 3) = -T(2) * near;
			return result;
		}
	}
	
	template <typename T, vector_options O>
	constexpr matrix4x4<T, O> translation(vector3<T, O> const& offset) {
		return {
			T(0), T(0), T(0), offset.__at(0),
			T(0), T(0), T(0), offset.__at(1),
			T(0), T(0), T(0), offset.__at(2),
			T(0), T(0), T(0), T(0)
		};
	}
	
	/// MARK: - Color
	template <typename = double4> struct colors;
	
	template <typename T, vector_options O>
	requires std::is_floating_point_v<T>
	struct colors<vector<T, 3, O>> {
		static constexpr vector<T, 3, O> hex(std::uint32_t value) {
			return colors<vector<T, 4, O>>::hex(value).swizzle(0, 1, 2);
		}
		
		static constexpr vector<T, 3, O> red    = colors<vector<T, 4, O>>::red.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> orange = colors<vector<T, 4, O>>::orange.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> yellow = colors<vector<T, 4, O>>::yellow.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> green  = colors<vector<T, 4, O>>::green.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> blue   = colors<vector<T, 4, O>>::blue.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> purple = colors<vector<T, 4, O>>::purple.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> pink   = colors<vector<T, 4, O>>::pink.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> black  = colors<vector<T, 4, O>>::black.swizzle(0, 1, 2);
		static constexpr vector<T, 3, O> white  = colors<vector<T, 4, O>>::white.swizzle(0, 1, 2);
	};
	
	template <typename T, vector_options O>
	requires std::is_floating_point_v<T>
	struct colors<vector<T, 4, O>> {
		static constexpr vector<T, 4, O> hex(std::uint32_t value) {
			value = mtl::__byte_swap(value);
			vector<std::uint8_t, 4, O> chars;
			std::memcpy(&chars, &value, sizeof(value));
			return vector<T, 4, O>(chars) / T((1 << 8) - 1);
		}
		
		static constexpr vector<T, 4, O> red    = { 1.0, 0.0, 0.0, 1.0 };
		static constexpr vector<T, 4, O> orange = { 1.0, 0.5, 0.0, 1.0 };
		static constexpr vector<T, 4, O> yellow = { 1.0, 1.0, 0.0, 1.0 };
		static constexpr vector<T, 4, O> green  = { 0.0, 1.0, 0.0, 1.0 };
		static constexpr vector<T, 4, O> blue   = { 0.0, 0.0, 1.0, 1.0 };
		static constexpr vector<T, 4, O> purple = { 0.5, 0.0, 1.0, 1.0 };
		static constexpr vector<T, 4, O> pink   = { 1.0, 0.0, 1.0, 1.0 };
		static constexpr vector<T, 4, O> black  = { 0.0, 0.0, 0.0, 1.0 };
		static constexpr vector<T, 4, O> white  = { 1.0, 1.0, 1.0, 1.0 };
	};
	
	
	/// MARK: - Shapes
	///
	///
	///
	/// MARK: - AABB
	template <typename T = double, std::size_t Dim = 3, vector_options O = vector_options{}>
	struct AABB {
		vector<T, Dim, O> origin;
		vector<T, Dim, O> extend;
	};
	
	template <typename T, std::size_t Dim, vector_options O>
	constexpr bool is_normalized(AABB<T, Dim, O> const& r) {
		return r.extend.map([](auto&& x) { return x >= 0; }).fold(mtl::__logical_and);
	}
	
	template <typename T, std::size_t Dim, vector_options O>
	constexpr AABB<T, Dim, O> normalize(AABB<T, Dim, O> const& r) {
		return {
			map(r.origin, r.extend, [](auto o, auto e) { return e < T(0) ? o + e : o; }),
			map(r.extend, mtl::__abs)
		};
	}
	
	template <typename T = double, vector_options O = vector_options{}>
	using rectangle = AABB<T, 2, O>;
	
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool operator==(AABB<T, Dim, O> r, AABB<U, Dim, P> s) {
		r = normalize(r);
		s = normalize(s);
		return r.origin == s.origin && r.extend == s.extend;
	}
	
	template <typename T, std::size_t Dim, vector_options O>
	constexpr T volume(AABB<T, Dim, O> const& r) {
		return fold(map(r.extend - r.origin, abs), mtl::__multiplies);
	}
	
	template <typename T, vector_options O>
	constexpr T area(rectangle<T, O> const& r) {
		return volume(r);
	}
	
	/// Test Box - Point Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(AABB<T, Dim, O> r, vector<U, Dim, P> const& p) {
		r = normalize(r);
		
		return map(r.origin, r.extend, p,
				   [](auto o, auto e, auto p) { return p >= o && p <= o + e; }).fold(mtl::__logical_and);
	}
	
	/// Test Point - Box Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(vector<U, Dim, P> const& p, AABB<T, Dim, O> r) {
		return do_intersect(r, p);
	}
	
	/// Test Box - Box Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(AABB<T, Dim, O> a, AABB<U, Dim, P> b) {
		a = normalize(a);
		b = normalize(b);
		return map(a.origin,
				   a.origin + a.extend,
				   b.origin,
				   b.origin + b.extend,
				   [](auto aMin,
					  auto aMax,
					  auto bMin,
					  auto bMax) { return aMin <= bMax && aMax >= bMin; }).fold(mtl::__logical_and);
	}
	
	/// MARK: Sphere
	template <typename T = double, std::size_t Dim = 3, vector_options O = vector_options{}.packed(true)>
	struct sphere {
		static_assert(std::is_floating_point<T>::value, "T needs to be floating point");
		static_assert(Dim >= 2);
		static_assert(Dim <= 4);
		
		vector<T, Dim, O> origin;
		T radius;
	};
	
	template <typename T = double, vector_options O = vector_options{}.packed(true)>
	using disk = sphere<T, 2, O>;
	
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool operator==(sphere<T, Dim, O> const& r, sphere<U, Dim, P> const& s) {
		return r.origin == s.origin && r.radius == s.radius;
	}
	
	template <typename T, std::size_t Dim, vector_options O>
	constexpr T volume(sphere<T, Dim, O> const& s) {
		if constexpr (Dim == 2) {
			return constants<T>::pi * __sqr(s.radius, 2);
		}
		else if constexpr (Dim == 3) {
			return T(4.0 / 3.0) * constants<T>::pi * __sqr(s.radius) * s.radius;
		}
		else {
			static_assert(Dim == 4);
			return T(0.5) * __sqr(constants<T>::pi) * __sqr(__sqr(s.radius));
		}
	}
	
	template <typename T, vector_options O>
	constexpr T area(sphere<T, 2, O> const& r) {
		return volume(r);
	}
	
	/// Test Sphere - Point Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(sphere<T, Dim, O> const& r, vector<U, Dim, P> const& p) {
		return distance(r.origin, p) <= r.radius;
	}
	
	/// Test Point - Sphere Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(vector<U, Dim, P> const& p, sphere<T, Dim, O> const& r) {
		return do_intersect(r, p);
	}
	
	/// Test Sphere - Sphere Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(sphere<T, Dim, O> const& a, sphere<U, Dim, P> const& b) {
		return distance(a.origin, b.origin) <= a.radius + b.radius;
	}
	
	/// Test Sphere - Box Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(sphere<T, Dim, O> const& sphere, AABB<U, Dim, P> box) {
		box = normalize(box);
		
		auto const closest_point_to_sphere = map(box.origin, box.origin + box.extend, sphere.origin,
												 [](auto bMin, auto bMax, auto sOrigin) {
			return std::max(bMin, std::min(sOrigin, bMax));
		});
		
		return do_intersect(sphere, closest_point_to_sphere);
	}
	
	/// Test Box - Sphere Intersection
	template <typename T, std::size_t Dim, vector_options O, typename U, vector_options P>
	constexpr bool do_intersect(AABB<U, Dim, P> a, sphere<T, Dim, O> b) {
		return do_intersect(b, a);
	}
	
	
	
	/// MARK: Line Segment
	template <typename T, std::size_t Dim, vector_options O = vector_options{}>
	struct line_segment {
		vector<T, Dim, O> begin;
		vector<T, Dim, O> end;
	};
	
	template <typename T = double, vector_options O = vector_options{}>
	using line_segment_2D = line_segment<T, 2, O>;
	
	template <typename T = double, vector_options O = vector_options{}>
	using line_segment_3D = line_segment<T, 3, O>;
	
	template <typename T, typename U, std::size_t Dim, vector_options O, vector_options P>
	auto distance(line_segment<T, Dim, O> const& l, vector<U, Dim, P> const& p) {
		// Return minimum distance between line segment l and point p
		using V = typename promote<T, U>::type;
		
		auto const l2 = distance_squared(l.begin, l.end);  // i.e. |w-v|^2 -  avoid a sqrt
		if (l2 == V(0)) {
			return distance(p, l.begin);   // v == w case
		}
		
	  // Consider the line extending the segment, parameterized as v + t (w - v).
	  // We find projection of point p onto the line.
	  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
	  // We clamp t to [0,1] to handle points outside the segment vw.
	  auto const t = std::max(T(0), std::min(T(1), dot(p - l.begin, l.end - l.begin) / l2));
	  auto const projection = l.begin + t * (l.end - l.begin);  // Projection falls on the segment
	  return distance(p, projection);
	}
	
	template <typename T, typename U, std::size_t Dim, vector_options O, vector_options P>
	auto distance(vector<U, Dim, P> const& p, line_segment<T, Dim, O> const& l) {
		return distance(l, p);
	}
	
	
}


/// MARK: - utl::iota<_VMTL::vector<...>>
namespace utl {
	
	template <typename>
	class iota;
	
	template <typename T, std::size_t N, _VMTL::vector_options O>
	requires std::is_integral_v<T>
	iota(_VMTL::vector<T, N, O>) -> iota<_VMTL::vector<T, N, O>>;
	
	template <typename T, typename U, std::size_t N, _VMTL::vector_options O, _VMTL::vector_options P>
	requires std::is_integral_v<T> && std::is_integral_v<U>
	iota(_VMTL::vector<T, N, O>, _VMTL::vector<U, N, P>) -> iota<_VMTL::vector<__mtl_promote(T, U), N, combine(O, P)>>;
	
	template <typename T, std::size_t N, _VMTL::vector_options O>
	requires std::is_integral_v<T>
	class iota<_VMTL::vector<T, N, O>> {
	public:
		using value_type = _VMTL::vector<T, N, O>;
		using size_type = _VMTL::vector<std::conditional_t<std::is_signed_v<T>, std::ptrdiff_t, std::size_t>, N, O>;
		
	public:
		class iterator {
			friend class iota;
			constexpr explicit iterator(value_type begin, value_type last):
				_begin(begin), _current(begin), _last(last) {}
			
		public:
			__mtl_interface_export
			constexpr value_type operator*() const { return _current; }
			__mtl_interface_export
			constexpr iterator& operator++() {
				for (std::size_t i = N - 1;;) {
					if (++_current[i] != _last[i]) {
						break;
					}
					_current[i] = _begin[i];
					
					if (i == 0) {
						_done = true;
						break;
					}
					--i;
				}
				
				return *this;
			}
			__mtl_interface_export
			constexpr bool operator!=(iterator const& rhs) const {
				__mtl_assert_audit(this->_begin == rhs._begin);
				__mtl_assert_audit(this->_last == rhs._last);
				return !_done;
			}
			
		private:
			value_type _begin, _current, _last;
			bool _done = false;
		};
		
	public:
		__mtl_interface_export
		constexpr iota(value_type last): _first(0), _last(last) {}
		__mtl_interface_export
		constexpr iota(value_type first, size_type last): _first(first), _last(last) {}
		
		__mtl_interface_export
		constexpr iterator begin() const {
			return iterator(_first, _last);
		}
		__mtl_interface_export
		constexpr iterator end() const {
			return iterator(_first, _last);
		}
		
		__mtl_interface_export
		constexpr size_type size() const { return _last - _first; }
		
	private:
		value_type _first;
		value_type _last;
	};
	
}

#endif // __MTL_EXT_HPP_INCLUDED__
