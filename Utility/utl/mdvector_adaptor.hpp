#pragma once

#include "base.hpp"

UTL(SYSTEM_HEADER)

#include "vector.hpp"
#include <mtl/mtl.hpp>
#include <utl/function_objects.hpp>

namespace utl {
	
	template <typename T, std::size_t Dim, typename Vector = utl::vector<T>, typename = utl::make_type_tuple<std::size_t, Dim>, typename = utl::make_index_sequence<Dim>>
	class mdvector_adaptor;
		
		
	template <typename T, std::size_t Dim, typename Vector, typename... SizeT, std::size_t... I>
	class mdvector_adaptor<T, Dim, Vector, utl::type_tuple<SizeT...>, utl::index_sequence<I...>>: Vector {
		
	public:
		using value_type = T;
		using size_type = mtl::vector<std::size_t, Dim>;
	
	public:
		mdvector_adaptor() = default;
		mdvector_adaptor(size_type size): Vector(size.fold(utl::multiplies)), _size(size) {}
		mdvector_adaptor(SizeT... size): mdvector_adaptor(size_type{ size... }) {}
		
		size_type size() const noexcept { return _size; }
		using Vector::data;
		using Vector::begin;
		using Vector::cbegin;
		using Vector::rbegin;
		using Vector::crbegin;
		using Vector::end;
		using Vector::cend;
		using Vector::rend;
		using Vector::crend;
		
		void resize(size_type size) {
			_size = size;
			Vector::resize(size.fold(utl::multiplies));
		}
		void resize(SizeT... size) {
			resize(size_type{ size... });
		}
		
		T& operator[](size_type const& i) noexcept {
			return utl::as_mutable(utl::as_const(*this)[i]);
		}
		T const& operator[](size_type const& i) const noexcept {
			return (*this)(i);
		}
		
		T& operator()(size_type const& i) noexcept {
			return utl::as_mutable(utl::as_const(*this)(i));
		}
		T const& operator()(size_type const& i) const noexcept {
			([&]{
				(void)I;
				utl_bounds_check(i[I], 0, _size[I]);
			}(), ...);
			if constexpr (Dim == 1) {
				return Vector::operator[](i.front());
			}
			else if constexpr (Dim == 2) {
				return Vector::operator[](i[0] + _size[0] * i[1]);
			}
			else if constexpr (Dim == 3) {
				return Vector::operator[](i[0] + _size[0] * i[1] + _size[0] * _size[1] * i[2]);
			}
			else {
				static_assert(Dim == 2, "not implemented");
			}
		}
		T& operator()(SizeT... i) noexcept { this->operator()(size_type{ i... }); }
		T const& operator()(SizeT... i) const noexcept { this->operator()(size_type{ i... }); }
		
	private:
		
		size_type _size{};
	};
	
}
