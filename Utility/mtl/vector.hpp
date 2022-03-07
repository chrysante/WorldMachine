#pragma once

#ifndef __MTL_VECTOR_HPP_INCLUDED__
#define __MTL_VECTOR_HPP_INCLUDED__

#include "__base.hpp"
_MTL_SYSTEM_HEADER_

#include "__common.hpp"
#define __MTL_DECLARE_VECTOR_TYPEDEFS__
#include "__typedefs.hpp"
#include "__arithmetic.hpp"

#include "__std_concepts.hpp"

#include <iosfwd>

/// MARK: - Synopsis
/*
 
 namespace mtl {
 
	template <typename T, std::size_t Size, vector_options Options>
	class vector {
		// type information:
		value_type = T
		size()      -> std::size_t    // Size
		data_size() -> std::size_t    // Size including padding
		options()   -> vector_options // Options
 
		// data members:
		T x, y, ...;
		T r, g, ...;
	 
		// contructors:
		vector() = default;
		vector(T value);                 -> // vector{ value, value, ... }
		vector(T value0, T value1, ...); -> // vector{ value, value, ... }
		vector(f: () -> T);              -> // vector{ f(), f(), ... }
		vector(f: (std::size_t) -> T);   -> // vector{ f(0), f(1), ... }
		
		// operator[]:
		operator[](std::size_t) -> T&
		operator[](std::size_t) const -> T const&
	 
		// begin, end:
		begin(), end()               -> T*
		begin() const, end() const   -> T const*
		cbegin() const, cend() const -> T const*
	 
		// data:
		data() -> T*
		data() const -> T const*
 
		// swizzle:
		swizzle(std::size_t i0, ...) -> vector<T, N, Options> // vector{ (*this)[i0], ... }
	 
		// map:
		map(f: (T) -> Any) const -> vector<{ deduced type }, Size, Options>
		 
		// fold:
		fold(f: (T, T) -> T) const -> T
		left_fold(f: (T, T) -> T) const -> T
		right_fold(f: (T, T) -> T) const -> T
	 
	 
		/// statics
		// unit:
		unit(std::size_t index, T value = 1) -> vector<T, Size, Options> // vector{ 0..., value [at position 'index'], 0... }
	};
	 
	operator==(vector, vector) -> bool
	map(vector<Ts, Size>..., f: (Ts...) -> fResult) -> vector<fResult, Size>
	fold(vector<T, Size> v, f: (T, T) -> T) -> T       // left_fold(v, f)
	left_fold(vector<T, Size> v, f: (T, T) -> T) -> T  // ...f(f(f(v[0], v[1]), v[2]),  ...)
	right_fold(vector<T, Size> v, f: (T, T) -> T) -> T // f(...f(..., f(v[N-2], v[N-1])))

 }
 
 */

namespace _VMTL {

	/// MARK: Map
	template <typename... T, std::size_t S, vector_options... O, _VMTL::invocable<T...> F>
	__mtl_mathfunction __mtl_always_inline
	constexpr auto __map_impl(F&& f, vector<T, S, O> const&... v) {
		using U = std::invoke_result_t<F, T...>;
		constexpr auto P = combine(O...);
		return vector<U, S, P>([&](std::size_t i) { return std::invoke(__mtl_forward(f), v.__at(i)...); });
	}
	
	template <typename T0, std::size_t S, vector_options O0, _VMTL::invocable<T0> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto map(vector<T0, S, O0> const& v0,
					   F&& f) requires(!std::is_same_v<void, std::invoke_result_t<F, T0>>) {
		return __map_impl(__mtl_forward(f), v0);
	}
	template <typename T0, typename T1, std::size_t S,
			  vector_options O0, vector_options O1,
			  _VMTL::invocable<T0, T1> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto map(vector<T0, S, O0> const& v0,
					   vector<T1, S, O1> const& v1,
					   F&& f) requires(!std::is_same_v<void, std::invoke_result_t<F, T0, T1>>) {
		return __map_impl(__mtl_forward(f), v0, v1);
	}
	template <typename T0, typename T1, typename T2, std::size_t S,
			  vector_options O0, vector_options O1, vector_options O2,
			  _VMTL::invocable<T0, T1, T2> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto map(vector<T0, S, O0> const& v0,
					   vector<T1, S, O1> const& v1,
					   vector<T2, S, O2> const& v2,
					   F&& f) requires(!std::is_same_v<void, std::invoke_result_t<F, T0, T1, T2>>) {
		return __map_impl(__mtl_forward(f), v0, v1, v2);
	}
	
	template <typename T0, typename T1, typename T2, typename T3, std::size_t S,
			  vector_options O0, vector_options O1, vector_options O2, vector_options O3,
			  _VMTL::invocable<T0, T1, T2, T3> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto map(vector<T0, S, O0> const& v0,
					   vector<T1, S, O1> const& v1,
					   vector<T2, S, O2> const& v2,
					   vector<T3, S, O3> const& v3,
					   F&& f) requires(!std::is_same_v<void, std::invoke_result_t<F, T0, T1, T2, T3>>) {
		return __map_impl(__mtl_forward(f), v0, v1, v2, v3);
	}
	
	template <typename T0, typename T1, typename T2, typename T3, typename T4, std::size_t S,
			  vector_options O0, vector_options O1, vector_options O2, vector_options O3, vector_options O4,
			  _VMTL::invocable<T0, T1, T2, T3, T4> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto map(vector<T0, S, O0> const& v0,
					   vector<T1, S, O1> const& v1,
					   vector<T2, S, O2> const& v2,
					   vector<T3, S, O3> const& v3,
					   vector<T4, S, O4> const& v4,
					   F&& f) requires(!std::is_same_v<void, std::invoke_result_t<F, T0, T1, T2, T3, T4>>) {
		return __map_impl(__mtl_forward(f), v0, v1, v2, v3, v4);
	}
	
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, std::size_t S,
			  vector_options O0, vector_options O1, vector_options O2, vector_options O3, vector_options O4, vector_options O5,
			  _VMTL::invocable<T0, T1, T2, T3, T4, T5> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto map(vector<T0, S, O0> const& v0,
					   vector<T1, S, O1> const& v1,
					   vector<T2, S, O2> const& v2,
					   vector<T3, S, O3> const& v3,
					   vector<T4, S, O4> const& v4,
					   vector<T5, S, O5> const& v5,
					   F&& f) requires(!std::is_same_v<void, std::invoke_result_t<F, T0, T1, T2, T3, T4, T5>>) {
		return __map_impl(__mtl_forward(f), v0, v1, v2, v3, v4, v5);
	}
	
	/// MARK: Fold
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto __left_fold_impl(auto&& f, auto&& a) {
		return a;
	}
	
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto __left_fold_impl(auto&& f, auto&& a, auto&& b) {
		return std::invoke(f, __mtl_forward(a), __mtl_forward(b));
	}
	
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto __left_fold_impl(auto&& f, auto&& a, auto&& b, auto&&... c) {
		return __left_fold_impl(f, std::invoke(f, __mtl_forward(a), __mtl_forward(b)), __mtl_forward(c)...);
	}
	
	template <typename T, std::size_t S, vector_options O, _VMTL::invocable_r<T, T, T> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr T left_fold(vector<T, S, O> const& v, F&& f) {
		return __mtl_with_index_sequence((I, S), {
			return __left_fold_impl(f, v.__at(I)...);
		});
	}
	
	template <typename T, std::size_t S, vector_options O, _VMTL::invocable_r<T, T, T> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr T right_fold(vector<T, S, O> const& v, F&& f) {
		return __mtl_with_index_sequence((I, S), {
			return __left_fold_impl(f, v.__at(S - 1 - I)...);
		});
	}
	
	template <typename T, std::size_t S, vector_options O, _VMTL::invocable_r<T, T, T> F>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr T fold(vector<T, S, O> const& v, F&& f) {
		return _VMTL::left_fold(v, __mtl_forward(f));
	}
	
	template <typename, std::size_t, vector_options, typename, typename>
	struct __vector_base;

	template <typename T, std::size_t Size, vector_options O>
	struct __vector_data;
	
	/// MARK: - struct __vector_data
	template <typename, std::size_t, vector_options>
	struct __vector_data_;
	
	/// MARK: Size = N
	template <typename T, std::size_t Size, vector_options O>
	struct __vector_data_ {
		__vector_data_() = default;
		constexpr __vector_data_(T const& a, T const& b, T const& c, T const& d, auto const&... r): __data{ a, b, c, d, r... } {}
		
		template <typename, std::size_t, vector_options, typename, typename>
		friend struct __vector_base;
		friend struct __vector_data<T, Size, O>;
		friend struct vector<T, Size, O>;
		friend struct quaternion<T>;
		
	private:
		union {
			T __data[Size];
			struct {
				T x;
				union {
					struct {
						T y;
						union {
							struct { T z, w; };
							_VMTL::vector<T, 2, O.packed(true)> zw;
						};
					};
					_VMTL::vector<T, 3, O.packed(true)> yzw;
					_VMTL::vector<T, 2, O.packed(true)> yz;
				};
			};
			_VMTL::vector<T, 3, O.packed(true)> xyz;
			_VMTL::vector<T, 2, O.packed(true)> xy;
			
			//  for quaternion
			struct {
				T real;
				union {
					_VMTL::vector<T, 3, O.packed(true)> vector;
					_VMTL::vector<T, 3, O.packed(true)> imag;
				};
			};
			
			struct {
				T r;
				union {
					struct {
						T g;
						union {
							struct { T b, a; };
							_VMTL::vector<T, 2, O.packed(true)> ba;
						};
					};
					_VMTL::vector<T, 3, O.packed(true)> gba;
					_VMTL::vector<T, 2, O.packed(true)> gb;
				};
			};
			_VMTL::vector<T, 3, O.packed(true)> rgb;
			_VMTL::vector<T, 2, O.packed(true)> rg;
		};
	};
	
	template <typename T, std::size_t Size, vector_options O>
	struct __vector_data: __vector_data_<T, Size, O> {
		using __base = __vector_data_<T, Size, O>;
		using __base::__base;
		
		using __base::x;
		using __base::xy;
		using __base::xyz;
		using __base::y;
		using __base::yz;
		using __base::yzw;
		using __base::z;
		using __base::zw;
		using __base::w;
		
		using __base::r;
		using __base::rg;
		using __base::rgb;
		using __base::g;
		using __base::gb;
		using __base::gba;
		using __base::b;
		using __base::ba;
		using __base::a;
	};
	
	/// MARK: Size = 3
	template <typename T, vector_options O>
	struct __vector_data_<T, 3, O> {
		__vector_data_() = default;
		constexpr __vector_data_(T const& a, T const& b, T const& c): __data{ a, b, c } {}
		
		template <typename, std::size_t, vector_options, typename, typename>
		friend struct __vector_base;
		friend struct __vector_data<T, 3, O>;
		friend struct vector<T, 3, O>;
		
	private:
		union {
			T __data[3];
			struct {
				T x;
				union {
					struct { T y, z; };
					vector<T, 2, O.packed(true)> yz;
				};
			};
			vector<T, 2, O.packed(true)> xy;
			
			struct {
				T r;
				union {
					struct { T g, b; };
					vector<T, 2, O.packed(true)> gb;
				};
			};
			vector<T, 2, O.packed(true)> rg;
		};
	};
	
	template <typename T, vector_options O>
	struct __vector_data<T, 3, O>: __vector_data_<T, 3, O> {
		using __base = __vector_data_<T, 3, O>;
		using __base::__base;
		
		using __base::x;
		using __base::xy;
		using __base::y;
		using __base::yz;
		using __base::z;
		
		using __base::r;
		using __base::rg;
		using __base::g;
		using __base::gb;
		using __base::b;
	};
	
	/// MARK: Size = 2
	template <typename T, vector_options O>
	struct __vector_data_<T, 2, O> {
		__vector_data_() = default;
		constexpr __vector_data_(T const& a, T const& b): __data{ a, b } {}
		
		template <typename, std::size_t, vector_options, typename, typename>
		friend struct __vector_base;
		friend struct __vector_data<T, 2, O>;
		friend struct vector<T, 2, O>;
		friend struct complex<T>;
		
	private:
		union {
			T __data[2];
			struct { T x, y; };
			struct { T r, g; };
			//  for complex
			struct { T real, imag; };
		};
	};
	
	template <typename T, vector_options O>
	struct __vector_data<T, 2, O>: __vector_data_<T, 2, O> {
		using __base = __vector_data_<T, 2, O>;
		using __base::__base;
		
		using __base::x;
		using __base::y;
		
		using __base::r;
		using __base::g;
	};
	
	/// MARK: - struct __vector_base
	constexpr std::size_t __calculate_alignment(std::size_t TAlign, std::size_t S, bool Packed) {
		auto const _s = [&]{
			switch (S) {
				case 0: // fallthrough
				case 1:
					__mtl_debugbreak("invalid");
					return -1;
				case 2:
					return 2;
				case 3:
					return 4;
				case 4:
					return 4;
				case 8:
					return 8;
					
				default:
					return 1;
			}
		}();
		
		return TAlign * (Packed ? 1 : _s);
	}
	
	template <typename T, std::size_t Size, vector_options O,
			  typename = __make_type_sequence<T, Size>,
			  typename = __make_index_sequence<Size>>
	class __vector_base;
	
	template <typename T, std::size_t Size, vector_options O,
			  typename... AllT, std::size_t... I>
	class alignas(__calculate_alignment(alignof(T), Size, O.packed()))
	__vector_base<T, Size, O, __type_sequence<AllT...>, __index_sequence<I...>>:
		public __vector_data<T, Size, O>
	{
		using __base = __vector_data<T, Size, O>;
		
	public:
		// constructors:
		__vector_base() = default;
		__mtl_always_inline __mtl_interface_export
		constexpr __vector_base(T const& x): __base{ ((void)I, x)... } {}
		__mtl_always_inline __mtl_interface_export
		constexpr __vector_base(AllT const&... x): __base{ x... } {}
		__mtl_always_inline __mtl_interface_export
		explicit constexpr __vector_base(_VMTL::invocable_r<T> auto&& f) noexcept(std::is_nothrow_invocable_v<decltype(f)>):
			__base{ ((void)I, std::invoke(f))... } {}
		__mtl_always_inline __mtl_interface_export
		explicit constexpr __vector_base(_VMTL::invocable_r<T, std::size_t> auto&& f) noexcept(std::is_nothrow_invocable_v<decltype(f), std::size_t>):
			__base{ std::invoke(f, I)... } {}
		
		template <_VMTL::convertible_to<T> U, vector_options P>
		__mtl_always_inline __mtl_interface_export
		constexpr __vector_base(vector<U, Size, P> const& rhs): __base{ static_cast<T>(rhs.__at(I))... } {}
	};

	/// MARK: - struct vector
	template <typename T, std::size_t Size, vector_options O>
	struct vector: public __vector_base<T, Size, O> {
		using __base = __vector_base<T, Size, O>;
	
		__mtl_always_inline
		constexpr T& __at(std::size_t i)& {
			__mtl_assert_audit(i < Size);
			return this->__data[i];
		}
		__mtl_always_inline
		constexpr T const& __at(std::size_t i) const& {
			__mtl_assert_audit(i < Size);
			return this->__data[i];
		}
		__mtl_always_inline
		constexpr T&& __at(std::size_t i)&& {
			__mtl_assert_audit(i < Size);
			return (T&&)(this->__data[i]);
		}
		__mtl_always_inline
		constexpr T const&& __at(std::size_t i) const&& {
			__mtl_assert_audit(i < Size);
			return (T const&&)(this->__data[i]);
		}
		
		/// MARK: Public Interface
		using value_type = T;
		__mtl_pure __mtl_always_inline __mtl_interface_export
		static constexpr std::size_t    size()      { return Size; }
		__mtl_pure __mtl_always_inline __mtl_interface_export
		static constexpr std::size_t    data_size() { return Size + (Size == 3 && !O.packed()); }
		__mtl_pure __mtl_always_inline __mtl_interface_export
		static constexpr vector_options options()   { return O; }
		
		static constexpr vector<T, Size, O> unit(std::size_t index, T const& value = 1) {
			__mtl_bounds_check(index, 0, Size);
			vector<T, Size, O> result{};
			result.__at(index) = value;
			return result;
		}
		
		/// Inheriting Constructors from __vector_base:
		using __base::__base;
		vector() = default;
		
		/// Some more Constructors
		// vector3(vector2, scalar)
		__mtl_always_inline __mtl_interface_export
		constexpr vector(vector<T, 2, O> const& a, T const& b): vector(a.__at(0), a.__at(1), b) {}
		// vector3(scalar, vector2)
		__mtl_always_inline __mtl_interface_export
		constexpr vector(T const& a, vector<T, 2, O> const& b): vector(a, b.__at(0), b.__at(1)) {}
		// vector4(vector2, s, s)
		__mtl_always_inline __mtl_interface_export
		constexpr vector(vector<T, 2, O> const& a, T const& b, T const& c): vector(a.__at(0), a.__at(1), b, c) {}
		// vector4(scalar, vector2, scalar)
		__mtl_always_inline __mtl_interface_export
		constexpr vector(T const& a, vector<T, 2, O> const& b, T const& c): vector(a, b.__at(0), b.__at(1), c) {}
		// vector4(scalar, scalar, vector2)
		__mtl_always_inline __mtl_interface_export
		constexpr vector(T const& a, T const& b, vector<T, 2, O> const& c): vector(a, b, c.__at(0), c.__at(1)) {}
		// vector4(vector2, vector2)
		__mtl_always_inline __mtl_interface_export
		vector(vector<T, 2, O> const& a, vector<T, 2, O> const& b): vector(a.__at(0), a.__at(1), b.__at(0), b.__at(1)) {}
		// vector(vector3, scalar)
		__mtl_always_inline __mtl_interface_export
		vector(vector<T, 3, O> const& a, T const& b): vector(a.__at(0), a.__at(1), a.__at(2), b) {}
		// vector4(scalar, vector3)
		__mtl_always_inline __mtl_interface_export
		vector(T const& a, vector<T, 3, O> const& b): vector(a, b.__at(0), b.__at(1), b.__at(2)) {}
		
		vector& operator=(vector const&)& = default;
		
		/// operator[]:
		__mtl_always_inline __mtl_interface_export
		constexpr T& operator[](std::size_t index)& { return const_cast<T&>(const_cast<vector const&>(*this)[index]); }
		__mtl_always_inline __mtl_interface_export
		constexpr T const& operator[](std::size_t index) const& { __mtl_bounds_check(index, 0, Size); return __at(index); }
		__mtl_always_inline __mtl_interface_export
		constexpr T&& operator[](std::size_t index)&& { return const_cast<T&&>(const_cast<vector const&&>(*this)[index]); }
		__mtl_always_inline __mtl_interface_export
		constexpr T const&& operator[](std::size_t index) const&& { __mtl_bounds_check(index, 0, Size); return std::move(__at(index)); }
		
		/// begin, end:
		__mtl_always_inline __mtl_interface_export
		constexpr T* begin()              { return this->__data; }
		__mtl_always_inline __mtl_interface_export
		constexpr T const* begin()  const { return this->__data; }
		__mtl_always_inline __mtl_interface_export
		constexpr T const* cbegin() const { return this->__data; }
		__mtl_always_inline __mtl_interface_export
		constexpr T* end()                { return this->__data + size(); }
		__mtl_always_inline __mtl_interface_export
		constexpr T const* end()  const   { return this->__data + size(); }
		__mtl_always_inline __mtl_interface_export
		constexpr T const* cend() const   { return this->__data + size(); }
		
		/// data:
		__mtl_always_inline __mtl_interface_export
		constexpr T* data() { return this->__data; }
		__mtl_always_inline __mtl_interface_export
		constexpr T const* data() const { return this->__data; }
		
		/// swizzle:
		template <_VMTL::convertible_to<std::size_t>... I>
		__mtl_always_inline __mtl_interface_export
		constexpr vector<T, sizeof...(I), O> swizzle(I... i) const { return { (*this)[i]... }; }
		
		/// map:
		__mtl_always_inline __mtl_interface_export
		constexpr auto map(_VMTL::invocable<T> auto&& f) const { return _VMTL::map(*this, __mtl_forward(f)); }
		
		/// fold:
		__mtl_always_inline __mtl_interface_export
		constexpr T fold(_VMTL::invocable_r<T, T, T> auto&& f) const { return _VMTL::fold(*this, __mtl_forward(f)); }
		
		/// MARK: Arithmetic Assignment
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator+=(vector const& rhs)& {
			*this = *this + rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator+=(T const& rhs)& {
			*this = *this + rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator-=(vector const& rhs)& {
			*this = *this - rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator-=(T const& rhs)& {
			*this = *this - rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator*=(vector const& rhs)& {
			*this = *this * rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator*=(T const& rhs)& {
			*this = *this * rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator/=(vector const& rhs)& {
			*this = *this / rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator/=(T const& rhs)& {
			*this = *this / rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator%=(vector const& rhs)& requires(std::is_integral_v<T>) {
			*this = *this % rhs;
			return *this;
		}
		__mtl_always_inline __mtl_interface_export
		constexpr vector& operator%=(T const& rhs)& requires(std::is_integral_v<T>) {
			*this = *this % rhs;
			return *this;
		}
	};
	
	/// deduction guide
	template <typename... T>
	vector(T...) -> vector<__mtl_promote(T...), sizeof...(T)>;
 
	
	/// MARK: - Operators
	/// operator==:
	template <typename T, typename U, std::size_t Size, vector_options O, vector_options P>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(vector<T, Size, O> const& v,
							  vector<U, Size, P> const& w) {
		return _VMTL::fold(_VMTL::map(v, w, _VMTL::__equals), _VMTL::__logical_and);
	}
	template <typename T, typename U, std::size_t Size, vector_options O>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(vector<T, Size, O> const& v,
							  U const& w) {
		return _VMTL::fold(_VMTL::map(v, [&](auto i) { return i == w; }), _VMTL::__logical_and);
	}
	template <typename T, typename U, std::size_t Size, vector_options O>
	requires requires(T&& t, U&& u) { { t == u } -> _VMTL::convertible_to<bool>; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr bool operator==(U const& w,
							  vector<T, Size, O> const& v) {
		return v == w;
	}
	
	// operator<<(std::ostream&, vector):
	template <typename T, std::size_t Size, vector_options O>
	__mtl_interface_export
	std::ostream& operator<<(std::ostream& _str, vector<T, Size, O> const& v) {
		/// To stop the compiler from checking operations on str, since only <iosfwd> is included
		auto& str = [](auto& x) -> decltype(auto) { return x; }(_str);
		str << '(';
		for (bool first = true; auto&& i: v) {
			str << (first ? ((void)(first = false), "") : ", ") << i;
		}
		return str << ')';
	}
	
	/// MARK: - Generic Vector Function
	/// Concatenate Vectors and Scalars, handrolled to go easy on template meta programming
	// (s, s)
	template <scalar T, scalar U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U), 2> concat(T const& a, U const& b) {
		return { a, b };
	}
	// (s, s, s)
	template <scalar T, scalar U, scalar V>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U, V), 3> concat(T const& a, U const& b, V const& c) {
		return { a, b, c };
	}
	// (s, s, s, s)
	template <scalar T, scalar U, scalar V, scalar W>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U, V, W), 4> concat(T const& a, U const& b, V const& c, W const& d) {
		return { a, b, c, d };
	}
	// (v2, s)
	template <scalar T, scalar U, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U), 3, O> concat(vector<T, 2, O> const& a, U const& b) {
		return { a, b };
	}
	// (s, v2)
	template <scalar T, scalar U, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U), 3, O> concat(T const& a, vector<U, 2, O> const& b) {
		return { a, b };
	}
	// (v2, s, s)
	template <scalar T, scalar U, scalar V, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U, V), 4, O> concat(vector<T, 2, O> const& a, U const& b, V const& c) {
		return { a, b, c };
	}
	// (s, v2, s)
	template <scalar T, scalar U, scalar V, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U, V), 4, O> concat(T const& a, vector<U, 2, O> const& b, V const& c) {
		return { a, b, c };
	}
	// (s, s, v2)
	template <scalar T, scalar U, scalar V, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U, V), 4, O> concat(T const& a, U const& b, vector<V, 2, O> const& c) {
		return { a, b, c };
	}
	// (v2, v2)
	template <scalar T, scalar U, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U), 4, O> concat(vector<T, 2, O> const& a, vector<U, 2, P> const& b) {
		return { a, b };
	}
	// (v3, s)
	template <scalar T, scalar U, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U), 4, O> concat(vector<T, 3, O> const& a, U const& b) {
		return { a, b };
	}
	// (s, v3)
	template <scalar T, scalar U, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	vector<__mtl_promote(T, U), 4, O> concat(T const& a, vector<U, 3, O> const& b) {
		return { a, b };
	}
	
	/// Reverse
	template <typename T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, Size, O> reverse(vector<T, Size, O> const& v) {
		return vector<T, Size, O>([&](std::size_t i) {
			return v.__at(Size - 1 - i);
		});
	}
	
	/// Type Cast
	template <typename To, typename T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<To, Size, O> type_cast(vector<T, Size, O> const& v) {
		return vector<To, Size, O>([&](std::size_t i) {
			return static_cast<To>(v.__at(i));
		});
	}
	
	/// Dimension Cast
	template <std::size_t To, typename T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, To, O> dimension_cast(vector<T, Size, O> const& v) {
		return vector<T, To, O>([&](std::size_t i) {
			if constexpr (To > Size) {
				return i >= Size ? T(0) : v.__at(i);
			}
			else {
				return v.__at(i);
			}
		});
	}
	
	/// MARK: - Vector Math Functions
	/// Dot Product of two Vectors
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto dot(vector<T, Size, O> const& a, vector<U, Size, P> const& b)
	{
		return fold(a * b, _VMTL::__plus);
	}
	
	/// Norm Squared of Vector (Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto norm_squared(vector<T, Size, O> const& a)
	{
		return map(a, [](auto const& x) { return norm_squared(x); }).fold(_VMTL::__plus);
	}
	
	/// Length Squared of Vector (same as Norm Squared, Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto length_squared(vector<T, Size, O> const& a)
	{
		return norm_squared(a);
	}
	
	/// Norm of Vector (Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto norm(vector<T, Size, O> const& a) {
		if constexpr (real_scalar<T>) {
			return __mtl_with_index_sequence((I, Size), {
				return _VMTL::hypot(a.__at(I)...);
			});
		}
		else {
			return __mtl_with_index_sequence((I, Size), {
				return _VMTL::hypot(norm(a.__at(I))...);
			});
		}
	}
	
	/// Fast / Unsafe Norm of Vector (Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto fast_norm(vector<T, Size, O> const& a) {
		if constexpr (real_scalar<T>) {
			return __mtl_with_index_sequence((I, Size), {
				return _VMTL::fast_hypot(a.__at(I)...);
			});
		}
		else {
			return __mtl_with_index_sequence((I, Size), {
				return _VMTL::fast_hypot(fast_norm(a.__at(I))...);
			});
		}
	}
	
	/// pNorm of Vector
	template <scalar F, scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto pnorm(F p, vector<T, Size, O> const& a) {
		if constexpr (real_scalar<T>) {
			return __mtl_with_index_sequence((I, Size), {
				return _VMTL::phypot(p, a.__at(I)...);
			});
		}
		else {
			return __mtl_with_index_sequence((I, Size), {
				return _VMTL::phypot(p, pnorm(p, a.__at(I))...);
			});
		}
	}
	
	/// Length of Vector (same as Norm, Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto length(vector<T, Size, O> const& a) {
		return norm(a);
	}
	
	/// Distance Squared between to Points (Euclidian)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto distance_squared(vector<T, Size, O> const& a, vector<U, Size, P> const& b) {
		return norm_squared(a - b);
	}

	/// Distance between to Points (Euclidian)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto distance(vector<T, Size, O> const& a, vector<U, Size, P> const& b) {
		return norm(a - b);
	}
	
	/// Fast / Unsafe Distance between to Points (Euclidian)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto fast_distance(vector<T, Size, O> const& a, vector<U, Size, P> const& b) {
		return fast_norm(a - b);
	}
	
	/// Normalize Vector (Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto normalize(vector<T, Size, O> const& a) {
		return a / norm(a);
	}
	
	/// Fast / Unsafe Normalize Vector (Euclidian)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto fast_normalize(vector<T, Size, O> const& a) {
		return a / fast_norm(a);
	}
	
	/// Element-wise Absolute Value of Vector
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto abs(vector<T, Size, O> const& a) {
		return a.map(_VMTL::__abs);
	}

	/// Element-wise Square Root of Vector
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto sqrt(vector<T, Size, O> const& a) {
		return a.map(_VMTL::__sqrt);
	}
	
	/// Element-wise Ceil Divide of Vector by Vector
	template <typename T, typename U = T, std::size_t Size, vector_options O, vector_options P = O>
	requires std::is_integral_v<T> && std::is_integral_v<U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto ceil_divide(vector<T, Size, O> const& a, vector<U, Size, P> const& b) {
		return map(a, b, [](auto&& x, auto&& y) { return _VMTL::__ceil_divide(x, y); });
	}
	
	/// Element-wise Ceil Divide of Vector by Scalar
	template <typename T, typename U, std::size_t Size, vector_options O>
	requires std::is_integral_v<T> && std::is_integral_v<U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto ceil_divide(vector<T, Size, O> const& a, U const& b) {
		return a.map([&](auto x){ return _VMTL::__ceil_divide(x, b); });
	}
	
	/// Element-wise Ceil Divide of Scalar by Vector
	template <typename T, typename U, std::size_t Size, vector_options O>
	requires std::is_integral_v<T> && std::is_integral_v<U>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto ceil_divide(T const& a, vector<U, Size, O> const& b) {
		return b.map([&](auto x){ return _VMTL::__ceil_divide(a, x); });
	}
	
	/// Element-wise Fractional Part of Vector
	template <typename T, std::size_t Size, vector_options O>
	requires std::is_floating_point_v<T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr auto fract(vector<T, Size, O> const& a) {
		return a.map([](auto&& x){ return _VMTL::__fract(x); });
	}
	
	/// Element-wise Floor of Vector
	template <typename T, std::size_t Size, vector_options O>
	requires std::is_floating_point_v<T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, Size, O> floor(vector<T, Size, O> const& a) {
		return a.map([](auto&& x){ return std::floor(x); });
	}
	
	/// Element-wise Ceil of Vector
	template <typename T, std::size_t Size, vector_options O>
	requires std::is_floating_point_v<T>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, Size, O> ceil(vector<T, Size, O> const& a) {
		return a.map([](auto&& x){ return std::ceil(x); });
	}
	
	/// Cross Product of two 3-Vectors
	template <real_scalar T, real_scalar U, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_interface_export
	constexpr vector3<__mtl_promote(T, U), combine(O, P)>
	cross(vector3<T, O> const& a, vector3<U, P> const& b) {
		return {
			a.__at(1) * b.__at(2) - a.__at(2) * b.__at(1),
			a.__at(2) * b.__at(0) - a.__at(0) * b.__at(2),
			a.__at(0) * b.__at(1) - a.__at(1) * b.__at(0)
		};
	}
	
}

/// MARK: - std::hash Specialization
template <typename T, std::size_t N, mtl::vector_options O>
class std::hash<mtl::vector<T, N, O>> {
public:
	std::size_t operator()(mtl::vector<T, N, O> const& v) const {
		std::size_t seed = _VMTL::__hash_seed;
		for (auto& i: v) {
			seed = _VMTL::__hash_combine(seed, i);
		}
		return seed;
	}
};

/// MARK: - Decomposition
namespace _VMTL {
	
	template <std::size_t I, typename T, std::size_t Size, vector_options O>
	T const& get(vector<T, Size, O> const& v) {
		static_assert(I < Size);
		return v.__at(I);
	}
	template <std::size_t I, typename T, std::size_t Size, vector_options O>
	T& get(vector<T, Size, O>& v) {
		static_assert(I < Size);
		return v.__at(I);
	}
	template <std::size_t I, typename T, std::size_t Size, vector_options O>
	T&& get(vector<T, Size, O>&& v) {
		static_assert(I < Size);
		return std::move(v).__at(I);
	}
	template <std::size_t I, typename T, std::size_t Size, vector_options O>
	T const&& get(vector<T, Size, O> const&& v) {
		static_assert(I < Size);
		return std::move(v).__at(I);
	}
	
}

template <typename T, std::size_t Size, _VMTL::vector_options O>
struct std::tuple_size<_VMTL::vector<T, Size, O>>: std::integral_constant<std::size_t, Size> {};

template <std::size_t I, typename T, std::size_t Size, _VMTL::vector_options O>
struct std::tuple_element<I, _VMTL::vector<T, Size, O>> {
	using type = T;
};

#endif // __MTL_VECTOR_HPP_INCLUDED__
