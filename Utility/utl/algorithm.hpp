#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "type_traits.hpp"
#include <cstddef>
#include <tuple>
#include <algorithm>
#include <limits>
#include <iosfwd>
#include "concepts.hpp"

namespace utl {
	
//	template <typename R>
//	concept range = requires(R&& r) {
//		r.begin();
//		r.end();
//	};
//
//
//	std::size_t size(range auto&& r) {
//		return r.size();
//	}
//
//	void reserve(range auto&& r, std::size_t cap) {
//		r.reserve(cap);
//	}
//
//	std::ostream& operator<<(std::ostream& str, range auto const& r) {
//		bool first = true;
//		for (auto&& i: r) {
//			str << (first ? (first = false, "") : ", ") << i;
//		}
//		return str;
//	}
//
//	template <typename F, range... R>
//	class map_view {
//		template <typename R_>
//		using _iter_t = decltype(std::declval<R_ const>().begin());
//	public:
//		class iterator {
//			friend class map_view;
//			iterator(auto& f, auto itr): _f(f), _itr(itr) {
//
//			}
//
//		public:
//			bool operator!=(iterator const& rhs) const {
//				return UTL_WITH_INDEX_SEQUENCE((I, sizeof...(R)), {
//					return (... && (std::get<I>(this->_itr) != std::get<I>(rhs._itr)));
//				});
//			}
//
//			auto operator*() const {
//				return UTL_WITH_INDEX_SEQUENCE((I, sizeof...(R)), {
//					return _f(*std::get<I>(_itr)...);
//				});
//			}
//
//			iterator& operator++() {
//				UTL_WITH_INDEX_SEQUENCE((I, sizeof...(R)), {
//					(++std::get<I>(_itr), ...);
//				});
//				return *this;
//			}
//
//		private:
//			F const& _f;
//			std::tuple<_iter_t<R>...> _itr;
//		};
//		using const_iterator = iterator;
//	public:
//		map_view(R const&... r, auto&& f):
//			_f(f), _ranges(r...) {}
//
//		const_iterator begin() const {
//			return const_iterator(_f, UTL_WITH_INDEX_SEQUENCE((I, sizeof...(R)), {
//				return std::tuple(std::get<I>(_ranges).begin()...);
//			}));
//		}
//
//		const_iterator end() const {
//			return const_iterator(_f, UTL_WITH_INDEX_SEQUENCE((I, sizeof...(R)), {
//				return std::tuple<_iter_t<R>...>(std::get<I>(_ranges).end()...);
//			}));
//		}
//
//	private:
//		F _f;
//		std::tuple<R const&...> _ranges;
//	};
//
//
//	template <range R, typename F>
//	map_view(R, F) -> map_view<F, R>;
//
//	template <range R0, range R1, typename F>
//	map_view(R0, R1, F) -> map_view<F, R0, R1>;
//
//
	template <typename T>
	class iota;

	template <integral T>
	class iota<T> {
	public:
		using value_type = T;
		using size_type = std::conditional_t<std::is_signed_v<T>, std::ptrdiff_t, std::size_t>;

	protected:
		template <typename InternalRep, typename ExternalRep>
		class Iterator {
			friend class iota;
			template <typename, typename>
			friend class Iterator;
			constexpr explicit Iterator(InternalRep value): _value(value) {}
			
		public:
			template <typename ExternalRepRhs>
			constexpr Iterator(Iterator<InternalRep, ExternalRepRhs> const& other): _value(other._value) {}

		public:
			constexpr ExternalRep operator*() const { return static_cast<ExternalRep>(_value); }
			constexpr Iterator& operator++() {
				++_value;
				return *this;
			}
			constexpr bool operator!=(Iterator const& rhs) const {
				return this->_value != rhs._value;
			}

		private:
			InternalRep _value;
		};
		
	public:
		using iterator = Iterator<T, T>;

	public:
		constexpr iota(T last): _first(0), _last(last) {}
		constexpr iota(T begin, T last): _first(begin), _last(last) {}

		constexpr iterator begin() const {
			return iterator(_first);
		}
		constexpr iterator end() const {
			return iterator(_last);
		}

		constexpr size_type size() const { return _last - _first; }

	private:
		T _first;
		T _last;
	};

	template <typename E>
	requires(std::is_enum_v<E>)
	class iota<E>: iota<std::underlying_type_t<E>> {
		using T = std::underlying_type_t<E>;
		using _base = iota<T>;
	public:
		constexpr iota(E size): _base(static_cast<T>(size)) {}
		constexpr iota(E first, E last): _base(static_cast<T>(first), static_cast<T>(last) - static_cast<T>(first)) {}
		using iterator = typename _base::template Iterator<std::underlying_type_t<E>, E>;

		constexpr iterator begin() const {
			return this->_base::begin();
		}
		constexpr iterator end() const {
			return this->_base::end();
		}

		using _base::size;
	};
	
	template <integral T>
	iota(T) -> iota<T>;

	template <integral T, convertible_to<std::conditional_t<std::is_signed_v<T>, std::ptrdiff_t, std::size_t>> S>
	iota(T, S) -> iota<T>;
	
	template <typename T, std::size_t Size>
	std::size_t array_size(T const(&)[Size]) { return Size; }
	
	
	enum struct direction {
		forward, backward
	};
	
	inline constexpr auto forward = direction::forward;
	inline constexpr auto backward = direction::backward;
	
	template <typename Range, UTL_ENABLE_IF(is_range<Range>::value),
			  typename F>
	void for_each(Range& r, F&& f) {
		auto const end = r.end();
		for (auto i = r.begin(); i != end; ++i)
			f(*i);
	}
	
	/*
	 *	f should return true to break, false to continue, or void to not break at all
	 *  returns true if loop has been broken
	 */
	template <direction dir = forward, typename... Range, UTL_ENABLE_IF((is_range<Range>::value && ...)),
			  typename F>
	auto find_if(F&& f, Range&&... r) {
		auto const invoke_f = [&](std::size_t index, auto&&... args) {
			if constexpr (std::is_invocable<F, decltype(args)...>::value) {
				static_assert(std::is_constructible<bool, decltype(f(std::forward<decltype(args)>(args)...))>::value);
				return f(std::forward<decltype(args)>(args)...);
			}
			else {
				static_assert((std::is_invocable<F, std::size_t, decltype(args)...>::value));
				static_assert(std::is_constructible<bool, decltype(f(std::size_t{}, std::forward<decltype(args)>(args)...))>::value);
				return f(index, std::forward<decltype(args)>(args)...);
			}
		};
		
		auto const empty_invoke_result = [&]{
			return decltype(invoke_f(0, *r.begin()...)){};
		};
		
		if constexpr (dir == direction::forward) {
			return [&](auto... i) {
				auto const end = std::min({ (r.end() - r.begin())... });
				for (std::size_t index = 0; index != end; ++index, (++i, ...)) {
					if (auto result = invoke_f(index, *i...)) {
						return result;
					}
				}
				return empty_invoke_result();
			}(r.begin()...);
		}
		else {
			static_assert(dir == direction::backward);
			return [&](auto... i) {
				auto const begin = std::min({ (r.end() - r.begin())... });
				for (std::ptrdiff_t index = static_cast<std::ptrdiff_t>(begin) - 1;
					 index >= 0;
					 --index, (++i, ...))
				{
					if (auto result = invoke_f(index, *i...)) {
						return result;
					}
				}
				return empty_invoke_result();
			}(std::reverse_iterator(r.end())...);
		}
	}
	
	template <direction dir = forward, typename... Range, UTL_ENABLE_IF((is_range<Range>::value && ...)),
			  typename F>
	void for_each(F&& f, Range&&... r) {
		auto const invoke_f = [&](std::size_t index, auto&&... args) {
			if constexpr (std::is_invocable<F, decltype(args)...>::value) {
				f(std::forward<decltype(args)>(args)...);
			}
			else {
				static_assert((std::is_invocable<F, std::size_t, decltype(args)...>::value));
				f(index, std::forward<decltype(args)>(args)...);
			}
		};
		
		if constexpr (dir == direction::forward) {
			return [&](auto... i) {
				auto const end = std::min({ (r.end() - r.begin())... });
				for (std::size_t index = 0; index != end; ++index, (++i, ...)) {
					invoke_f(index, *i...);
				}
			}(r.begin()...);
		}
		else {
			static_assert(dir == direction::backward);
			return [&](auto... i) {
				auto const begin = std::min({ (r.end() - r.begin())... }) - 1;
				auto const end   = std::numeric_limits<std::size_t>::max();
				for (std::size_t index = begin; index != end; --index, (++i, ...)) {
					invoke_f(index, *i...);
				}
			}(std::reverse_iterator(r.end())...);
		}
	}
	
	template <typename T, std::size_t Size,
			  typename F>
	void for_each(T (&array)[Size], F&& f) {
		auto const end = array + Size;
		for (auto i = array; i != end; ++i)
			f(*i);
	}
	
	template <typename It, typename F, UTL_ENABLE_IF(is_iterator<It>::value)>
	void for_each(It begin, It end, F&& f) {
		for (; begin != end; ++begin) {
			f(*begin);
		}
	}
	
	template <typename It1, typename It2, typename F, UTL_ENABLE_IF(is_iterator<It1>::value), UTL_ENABLE_IF(is_iterator<It2>::value)>
	void for_each(It1 begin1, It1 end1, It2 begin2, F&& f) {
		for (; begin1 != end1; ++begin1, ++begin2) {
			f(*begin1, *begin2);
		}
	}
	
	
	
	
	
	
}
