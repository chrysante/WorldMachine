#pragma once

#ifndef __MTL_ARITHMETIC_HPP_INCLUDED__
#define __MTL_ARITHMETIC_HPP_INCLUDED__

#include "__base.hpp"
_MTL_SYSTEM_HEADER_

#include "__common.hpp"

namespace _VMTL {
	
	/// MARK: - Vector Arithmetic
	///
	///
	///
	/// MARK: Plus
	/// Unary Plus(Vector)
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, Size, O> operator+(vector<T, Size, O> const& v) {
		return v;
	}
	
	/// Add Vector to Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, combine(O, P)>
	operator+(vector<T, Size, O> const& a,
			  vector<U, Size, P> const& b) {
		return map(a, b, _VMTL::__plus);
	}
	
	/// Add Scalar to Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator+(vector<T, Size, O> const& a,
			  U const& b) {
		return map(a, [&b](auto _a) {
			return _a + b;
		});
	}
	
	/// Add Vector to Scalar (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator+(T const& a,
			  vector<U, Size, O> const& b) {
		return map(b, [&a](auto b) { return a + b; });
	}

	/// MARK: Minus
	/// Negate Vector
	template <scalar T, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<T, Size, O> operator-(vector<T, Size, O> const& v) {
		return map(v, _VMTL::__minus);
	}
	
	/// Subract Vector from Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, combine(O, P)>
	operator-(vector<T, Size, O> const& a,
			  vector<U, Size, P> const& b) {
		return map(a, b, _VMTL::__minus);
	}

	/// Subract Scalar from Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator-(vector<T, Size, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a - b; });
	}

	/// Subract Vector from Scalar (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator-(T const& a,
			  vector<U, Size, O> const& b) {
		return map(b, [&a](auto b) { return a - b; });
	}

	/// MARK: Multiply
	/// Multiply Vector by Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, combine(O, P)>
	operator*(vector<T, Size, O> const& a,
			  vector<U, Size, P> const& b) {
		return map(a, b, _VMTL::__multiplies);
	}

	/// Multiply Vector by Scalar (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator*(vector<T, Size, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a * b; });
	}

	/// Multiply Scalar by Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator*(T const& a,
			  vector<U, Size, O> const& b) {
		return map(b, [&a](auto b) { return a * b; });
	}
	
	/// MARK: Divide
	/// Divide Vector by Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, combine(O, P)>
	operator/(vector<T, Size, O> const& a,
			  vector<U, Size, P> const& b) {
		return map(a, b, _VMTL::__divides);
	}

	/// Divide Vector by Scalar (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator/(vector<T, Size, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a / b; });
	}

	/// Divide Scalar by Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator/(T const& a,
			  vector<U, Size, O> const& b) {
		return map(b, [&a](auto b) { return a / b; });
	}

	/// MARK: Modulo
	/// Modulo divide Vector by Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O, vector_options P>
	requires requires(T&& t, U&& u) { t % u; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, combine(O, P)>
	operator%(vector<T, Size, O> const& a,
			  vector<U, Size, P> const& b) {
		return map(a, b, _VMTL::__modulo);
	}

	/// Modulo divide Vector by Scalar (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	requires requires(T&& t, U&& u) { t % u; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator%(vector<T, Size, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a % b; });
	}

	/// Modulo divide Scalar by Vector (element-wise)
	template <scalar T, scalar U, std::size_t Size, vector_options O>
	requires requires(T&& t, U&& u) { t % u; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), Size, O>
	operator%(T const& a,
			  vector<U, Size, O> const& b) {
		return map(b, [&a](auto b) { return a % b; });
	}
	
	/// MARK: - Matrix Arithmetic
	///
	///
	///
	/// MARK: Plus
	/// Unary Plus(Matrix)
	template <scalar T, std::size_t Rows, std::size_t Columns, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<T, Rows, Columns, O> operator+(matrix<T, Rows, Columns, O> const& v) {
		return v;
	}
	
	/// Add Matrix to Matrix
	template <scalar T, scalar U, std::size_t Rows, std::size_t Columns, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), Rows, Columns, combine(O, P)>
	operator+(matrix<T, Rows, Columns, O> const& a,
			  matrix<U, Rows, Columns, P> const& b) {
		return map(a, b, _VMTL::__plus);
	}
	
	/// MARK: Minus
	/// Negate Matrix
	template <scalar T, std::size_t Rows, std::size_t Columns, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<T, Rows, Columns, O> operator-(matrix<T, Rows, Columns, O> const& v) {
		return map(v, _VMTL::__minus);
	}
	
	/// Subtract Matrix from Matrix
	template <scalar T, scalar U, std::size_t Rows, std::size_t Columns, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), Rows, Columns, combine(O, P)>
	operator-(matrix<T, Rows, Columns, O> const& a,
			  matrix<U, Rows, Columns, P> const& b) {
		return map(a, b, _VMTL::__minus);
	}
	
	/// MARK: Multiply
	/// Multiply Matrix by Scalar
	template <scalar T, scalar U, std::size_t Rows, std::size_t Columns, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), Rows, Columns, O>
	operator*(matrix<T, Rows, Columns, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a * b; });
	}
	
	/// Multiply Scalar by Matrix
	template <scalar T, scalar U, std::size_t Rows, std::size_t Columns, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), Rows, Columns, O>
	operator*(T const& a,
			  matrix<U, Rows, Columns, O> const& b) {
		return map(b, [&a](auto b) { return a * b; });
	}
	
	/// Multiply Matrix by Matrix
	template <scalar T, scalar U,
			  std::size_t RowsA, std::size_t ColumnsA, std::size_t ColumnsB,
			  vector_options O, vector_options P>
	__mtl_mathfunction __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), RowsA, ColumnsB, combine(O, P)>
	operator*(matrix<T, RowsA, ColumnsA, O> const& A,
			  matrix<U, ColumnsA, ColumnsB, P> const& B) {
		return matrix<__mtl_promote(T, U), RowsA, ColumnsB, combine(O, P)>([&](std::size_t i, std::size_t j) {
			return __mtl_with_index_sequence((K, ColumnsA), {
				return ((A.__at(i, K) * B.__at(K, j)) + ...);
			});
		});
	}

	/// Multiply Matrix by Vector
	template <scalar T, scalar U, std::size_t RowsA, std::size_t ColumnsA, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), RowsA, combine(O, P)>
	operator*(matrix<T, RowsA, ColumnsA, O> const& A,
			  vector<U, ColumnsA, P> const& v) {
		return vector<__mtl_promote(T, U), RowsA, combine(O, P)>([&](std::size_t i) {
			return __mtl_with_index_sequence((K, ColumnsA), {
				return ((A.__at(i, K) * v.__at(K)) + ...);
			});
		});
	}

	/// Multiply Vector by Matrix
	template <scalar T, scalar U, std::size_t ColumnsA, std::size_t ColumnsB, vector_options O, vector_options P>
	__mtl_mathfunction __mtl_interface_export
	constexpr vector<__mtl_promote(T, U), ColumnsB, combine(O, P)>
	operator*(vector<T, ColumnsA, O> const& v,
			  matrix<U, ColumnsA, ColumnsB, P> const& A) {
		return vector<__mtl_promote(T, U), ColumnsB, combine(O, P)>([&](std::size_t j) {
			return __mtl_with_index_sequence((K, ColumnsA), {
				return ((v.__at(K) * A.__at(K, j)) + ...);
			});
		});
	}
	
	/// MARK: Divide
	/// Divide Matrix by Scalar
	template <scalar T, scalar U, std::size_t Rows, std::size_t Columns, vector_options O>
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), Rows, Columns, O>
	operator/(matrix<T, Rows, Columns, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a / b; });
	}

	/// MARK: Modulo
	/// Modulo divide Matrix by Scalar (element-wise)
	template <scalar T, scalar U, std::size_t Rows, std::size_t Columns, vector_options O>
	requires requires(T&& t, U&& u) { t % u; }
	__mtl_mathfunction __mtl_always_inline __mtl_interface_export
	constexpr matrix<__mtl_promote(T, U), Rows, Columns, O>
	operator%(matrix<T, Rows, Columns, O> const& a,
			  U const& b) {
		return map(a, [&b](auto a) { return a % b; });
	}
	
}

#endif // __MTL_ARITHMETIC_HPP_INCLUDED__
