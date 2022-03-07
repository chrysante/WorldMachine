#pragma once


#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "debug.hpp"
#include "memory_resource.hpp"
#include "range.hpp"
#include "algorithm.hpp"
#include "bit.hpp"
#include "concepts.hpp"

#include <cstddef>
#include <tuple>
#include <span>

namespace utl {
	
	namespace soa {
		
		template <typename T, typename UpdateFunction = void>
		struct element: UpdateFunction {
			static constexpr bool has_update_function = true;
			
			template <typename... Args>
			decltype(auto) execute(Args&&... args) { return static_cast<UpdateFunction&>(*this)(std::forward<Args>(args)...); }
			
			template <typename... Args>
			decltype(auto) execute(Args&&... args) const { return static_cast<UpdateFunction const&>(*this)(std::forward<Args>(args)...); }
			
			T value;
		};
		
		template <typename T>
		struct element<T, void> {
			static constexpr bool has_update_function = false;
			T value;
		};
		
		namespace _private {
			template <typename T>
			struct make_element { using type = soa::element<T, void>; };
			
			template <typename T, typename F>
			struct make_element<soa::element<T, F>> { using type = soa::element<T, F>; };
		}
		
		template <typename... T>
		class tuple_impl;
		
		template <typename... T, typename... F>
		class tuple_impl<element<T, F>...> {
			static_assert(is_unique<T...>::value);
			
		public:
			tuple_impl() = default;
			tuple_impl(T const&... t): m_values(t...) {}
			
			template <typename U, UTL_ENABLE_IF(contains<U, T...>::value)>
			U& get() { return std::get<first_index_of<U, T...>::value>(m_values); }
			template <typename U, UTL_ENABLE_IF(contains<U, T...>::value)>
			U const& get() const { return std::get<first_index_of<U, T...>::value>(m_values); }
		
		private:
			std::tuple<T...> m_values;
		};
		
		template <typename... T>
		using tuple = tuple_impl<typename _private::make_element<T>::type...>;
		
	}
	
	template <typename>
	class structure_of_arrays;
	
	template <typename... T, typename... F>
	class structure_of_arrays<soa::tuple_impl<soa::element<T, F>...>> {
		static_assert(is_unique<T...>::value);
	
		using _value_type = soa::tuple_impl<soa::element<T, F>...>;
		
	public:
		template <typename U>
		static constexpr auto index_of = first_index_of<U, T...>::value;
		
		template <typename U>
		struct contains: utl::contains<U, T...> {};
		
		template <typename U>
		using function_type = typename std::tuple_element<index_of<U>, std::tuple<F...>>::type;
		
	private:
		template <bool is_const>
		class reference_t {
		public:
			template <UTL_ENABLE_MEMBER_IF(!is_const)>
			explicit reference_t(T&... t): m_values(&t...) {}
			
			template <UTL_ENABLE_MEMBER_IF(is_const)>
			explicit reference_t(T const&... t): m_values(&t...) {}
			
			template <typename U, UTL_ENABLE_IF(contains<U>::value), UTL_ENABLE_MEMBER_IF(!is_const)>
			U& get() { return *std::get<index_of<U>>(m_values); }
			template <typename U, UTL_ENABLE_IF(contains<U>::value)>
			U const& get() const { return *std::get<index_of<U>>(m_values); }
			
			template <UTL_ENABLE_MEMBER_IF(!is_const)>
			operator reference_t<!is_const>() const { return utl::unsafe_bit_cast<reference_t<true>>(*this); }
			
			_value_type copy() const {
				return { this->get<T>()... };
			}
			
			_value_type move() const {
				return { std::move(this->get<T>())... };
			}
			
		private:
			std::tuple<typename std::conditional<is_const, T const, T>::type*...> m_values;
		};
		
	public:
		using value_type = _value_type;
		using reference = reference_t<false>;
		using const_reference = reference_t<true>;
		
	public:
		// MARK: - constructors
		structure_of_arrays() {
			((this->get_array_ptr<T>() = nullptr), ...);
		}
		
		structure_of_arrays(pmr::memory_resource* r): structure_of_arrays() {
			set_allocator(r);
		}
		
		// MARK: - destructor
		~structure_of_arrays() { destroy(); }
		
		// MARK: - observers
		std::size_t size() const { return m_size; }
		std::size_t capacity() const { return m_cap; }
		bool empty() const { return size() == 0; }
		
		// MARK: - allocator stuff
		void set_allocator(utl::pmr::memory_resource* r) {
			(set_allocator<T>(r), ...);
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		void set_allocator(pmr::polymorphic_allocator<U> alloc) {
			utl_ensure(this->get_allocator<U>() == alloc || this->capacity() == 0, "Allocator must compare equal to previously set one or container must be deallocated");
			this->template get_allocator<U>().~polymorphic_allocator<U>();
			new (&this->template get_allocator<U>()) pmr::polymorphic_allocator<U>(alloc);
		}
		
		// MARK: - modifiers
		void push_back(value_type const& elem) {
			this->push_back(elem.template get<T>()...);
		}
		void push_back(const_reference elem) {
			this->push_back(elem.template get<T>()...);
		}
		template <typename... U, UTL_ENABLE_IF(sizeof...(T) == sizeof...(U)), UTL_ENABLE_IF((std::is_convertible<U, T>::value && ...))>
		void push_back(U&&... u) {
			if (this->size() == this->capacity()) {
				grow();
			}
			((new (this->template get_array_ptr<T>() + m_size) T(std::forward<U>(u))), ...);
			(execute_update_function<T>(get_array_ptr<T>(), m_size), ...);
			++m_size;
		}
		
		void reserve(std::size_t n) {
			if (n > capacity()) {
				grow_to(n);
			}
		}
		
		void pop_back() {
			([&] {
				auto const buffer = this->template get_array_ptr<T>();
				// destroy last element
				(buffer + size() - 1)->~T();
			}(), ...);
			--m_size;
		}
		
		void erase(std::size_t index) {
			utl_bounds_check(index, 0, size());
			([&] {
				auto const buffer = this->template get_array_ptr<T>();
				auto const elem = buffer + index;
			    
				// shift following elements
				utl::for_each(elem, buffer + size() - 1, elem + 1, [](auto& a, auto& b) {
					a = std::move(b);
				});
				
				execute_update_function<T>(buffer, utl::range<std::size_t>(index, size() - 1));
				
				// destroy last element
				(buffer + size() - 1)->~T();
			}(), ...);
			--m_size;
		}
		
		void clear() {
			(for_each<T>([](T& t){
				t.~T();
			}), ...);
			m_size = 0;
		}
		
		void insert_at(std::size_t index, value_type const& elem) {
			insert_at(index, elem.template get<T>()...);
		}
		void insert_at(std::size_t index, const_reference elem) {
			insert_at(index, elem.template get<T>()...);
		}
		template <typename... U, UTL_ENABLE_IF(sizeof...(T) == sizeof...(U)), UTL_ENABLE_IF((std::is_convertible<U, T>::value && ...))>
		void insert_at(std::size_t index, U&&... u) {
			if (this->size() == this->capacity()) {
				grow();
			}
			([&] {
				auto const buffer = this->template get_array_ptr<T>();
				auto const elem = buffer + index;
				
				auto const back = buffer + size();
				// construct back from back - 1
				new (back) T(std::move(*(back - 1)));
				// shift
				for (auto i = back - 2, j = back - 1; i >= elem; --i, --j) {
					*j = std::move(*i);
				}
				// assign elem
				*elem = std::forward<U>(u);
				
				execute_update_function<T>(buffer, utl::range<std::size_t>(index, size()));
			}(), ...);
			++m_size;
		}
		
		// MARK: - algorithms
		template <typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto for_each(G&& f) {
			this->for_each<utl::direction::forward, U...>(std::forward<G>(f));
		}
		template <utl::direction dir, typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto for_each(G&& f) {
			utl::for_each<dir>(std::forward<G>(f), this->array<U>()...);
		}
		template <typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto for_each(G&& f) const {
			this->for_each<utl::direction::forward, U...>(std::forward<G>(f));
		}
		template <utl::direction dir, typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto for_each(G&& f) const {
			utl::for_each<dir>(std::forward<G>(f), this->array<U>()...);
		}
		
		template <utl::direction dir = utl::direction::forward>
		void for_each(utl::invocable<reference> auto&& f) {
			_for_each_impl(*this, UTL_FORWARD(f));
		}
		
		template <utl::direction dir = utl::direction::forward>
		void for_each(utl::invocable<const_reference> auto&& f) const {
			_for_each_impl(*this, UTL_FORWARD(f));
		}
		
	private:
		template <utl::direction dir = utl::direction::forward>
		static void _for_each_impl(auto&& soa, auto&& f) {
			if constexpr (dir == utl::direction::forward) {
				for (std::size_t i = 0, end = soa.size(); i < end; ++i) {
					f(soa[i]);
				}
			}
			else {
				for (std::size_t i = soa.size(); i > 0;) {
					--i;
					f(soa[i]);
				}
			}
		}
	public:
		
		template <typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto find_if(G&& f) {
			return this->find_if<utl::direction::forward, U...>(std::forward<G>(f));
		}
		template <utl::direction dir, typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto find_if(G&& f) {
			return utl::find_if<dir>(std::forward<G>(f), this->array<U>()...);
		}
		template <typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto find_if(G&& f) const {
			return this->find_if<utl::direction::forward, U...>(std::forward<G>(f));
		}
		template <utl::direction dir, typename ... U, typename G, UTL_ENABLE_IF((contains<U>::value && ...))>
		auto find_if(G&& f) const {
			return utl::find_if<dir>(std::forward<G>(f), this->array<U>()...);
		}
		
		// MARK: - accessors
		reference operator[](std::size_t index) {
			utl_bounds_check(index, 0, size());
			return reference(this->template get_array_ptr<T>()[index]...);
		}
		
		const_reference operator[](std::size_t index) const {
			utl_bounds_check(index, 0, size());
			return const_reference(this->template get_array_ptr<T>()[index]...);
		}

		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		std::span<U> array() {
			return std::span<U>(get_array_ptr<U>(), size());
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		std::span<U const> array() const {
			return std::span<U const>(get_array_ptr<U>(), size());
		}
		
		template <typename U, typename V, UTL_ENABLE_IF(contains<U>::value)>
		void set(V&& value, std::size_t index) {
			utl_bounds_check(index, 0, size());
			get_array_ptr<U>()[index] = std::forward<V>(value);
			execute_update_function<U>(get_array_ptr<U>(), index);
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		auto& update(std::size_t index) {
			utl_bounds_check(index, 0, size());
			execute_update_function<U>(get_array_ptr<U>(), index);
			return get_array_ptr<U>()[index];
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		U& get(std::size_t index) {
			utl_bounds_check(index, 0, size());
			return get_array_ptr<U>()[index];
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		U const& get(std::size_t index) const {
			utl_bounds_check(index, 0, size());
			return get_array_ptr<U>()[index];
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		U* data() {
			return this->template get_array_ptr<U>();
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		U const* data() const {
			return this->template get_array_ptr<U>();
		}
		
		
	private:
		void grow_to(std::size_t const new_cap) {
			bool const is_empty = empty();
			std::size_t const old_cap = capacity();
			
			([&] {
				auto& alloc = get_allocator<T>();
				T* const new_buffer = static_cast<T*>(alloc.allocate(new_cap));
				auto const old_buffer = this->template get_array_ptr<T>();
				utl::for_each(old_buffer, old_buffer + size(), new_buffer, [&](auto& i, auto& j) {
					new (&j) T(std::move(i));
					i.~T();
				});
				
				if (!is_empty) {
					alloc.deallocate(old_buffer, old_cap);
					execute_update_function<T>(new_buffer, range<std::size_t>(0, size()));
				}
				
				this->template set_array_ptr<T>(new_buffer);
			}(), ...);
			
			m_cap = new_cap;
		}
		
		void grow() {
			auto constexpr growth_factor = 2;
			std::size_t const new_cap = empty() ? 1 : capacity() * growth_factor;
			grow_to(new_cap);
		}
		
		void destroy() {
			if (!empty()) {
				((this->template get_allocator<T>().deallocate(this->get_array_ptr<T>(), capacity())), ...);
			}
		}
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		U*& get_array_ptr() { return std::get<index_of<U>>(m_arrays).value; }
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		U const* get_array_ptr() const { return std::get<index_of<U>>(m_arrays).value; }
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		void set_array_ptr(U* ptr) { std::get<index_of<U>>(m_arrays).value = ptr; }
		
		template <typename U, UTL_ENABLE_IF(contains<U>::value)>
		auto& get_allocator() { return std::get<index_of<U>>(m_alloc); }
		
		template <typename U, typename ... Args>
		void execute_update_function(Args&&... args) {
			using element_t = typename std::remove_reference<decltype(std::get<index_of<U>>(m_arrays))>::type;
			if constexpr (element_t::has_update_function) {
				std::get<index_of<U>>(m_arrays).execute(std::forward<Args>(args)...);
			}
		}
						  
	private:
		std::tuple<soa::element<T*, F>...> m_arrays;
		std::tuple<pmr::polymorphic_allocator<T>...> m_alloc;
		
		std::size_t m_size = 0, m_cap = 0;
	};
	
}

