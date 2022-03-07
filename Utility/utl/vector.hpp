#pragma once

#include "base.hpp"

UTL(SYSTEM_HEADER)

#include <type_traits>
#include <memory>
#include <algorithm>
#include <exception>
#include <bit>
#include <iosfwd>

#include "utility.hpp"
#include "concepts.hpp"
#include "__memory_resource_base.hpp"

/// Synopsis
/*
 
 namespace utl {
 
	 template <typename T, typename A = std::allocator<T>>
	 class vector {
	 public:
		// Implements the interface of std::vector<T, A>, with the exception that
		// iterator validity if not guaranteed after move operations. This enables the
		// implementation of small_vector<T, N, A> (see below).
	 };
	 
	 template <typename T, std::size_t N = default_inline_capacity<T, std::allocator<T>>, typename A = std::allocator<T>>
	 class small_vector: public vector<T, N> {
	 public:
		// Implements the same constructors and assignment operators as vector<T, A>.
		// Inherits the remaining functionality of vector<T, A>.
		
		// returns N
		static constexpr std::size_t inline_capacity();
	 
		// returns true iff contents is stored in local buffer within the object.
		bool uses_inline_buffer() const noexcept;
	 };
	 
	 // Holds the maximum value so that sizeof(small_vector<T>) == 64
	 template <typename T, typename A>
	 constexpr std::size_t default_inline_capacity;
 
	namespace pmr {
		template <typename T>
		using vector = utl::vector<T, polymorphic_allocator<T>>;
		template <typename T>
		using small_vector = utl::vector<T, default_inline_capacity<T, polymorphic_allocator<T>>, polymorphic_allocator<T>>;
	}
 
 }
 
 */


namespace utl {
	
	/// Edit or specialize this template to change the maximum capacity of utl::vector.
	/// Using 32 bit size type reduces the memory footprint of the vector object from 24 bytes to 16 bytes..
	/// With 32 bit size type and 1-byte data types the maximum capacity is 4GB, which might not suffice.
	/// So for 1-byte types the default size type is 64 bit wide.
	template <typename T>
	struct vector_size_type_selector {
		using type = std::conditional_t<sizeof(T) == 1, std::uint64_t, std::uint32_t>;
	};
	
	template <typename T, typename A>
	struct __vector_config;
	
	/// Just to define it later
	template <typename T, typename A> constexpr std::size_t __small_vector_default_inline_capacity();
	template <typename T, typename A> constexpr std::size_t default_inline_capacity = __small_vector_default_inline_capacity<T, A>();

	// MARK: - Forward Declarations
	template <typename T, typename Allocator = std::allocator<T>>
	class vector;
	
	template <typename T, std::size_t N = default_inline_capacity<T, std::allocator<T>>, typename Allocator = std::allocator<T>>
	class small_vector;
	
	namespace pmr {
		template <typename T>
		using vector = utl::vector<T, polymorphic_allocator<T>>;
		template <typename T, std::size_t N = default_inline_capacity<T, polymorphic_allocator<T>>>
		using small_vector = utl::small_vector<T, N, polymorphic_allocator<T>>;
	}
	
	
	// MARK: - class vector
	template <typename T, typename Allocator>
	class vector: private Allocator {
	public:
		/// MARK: Member Types
		using value_type = T;
		using allocator_type = Allocator;
		using size_type = typename __vector_config<T, Allocator>::size_type;
		using difference_type = std::ptrdiff_t;
		
		using reference = value_type&;
		using const_reference = value_type const&;
		using pointer = T*;
		using const_pointer = T const*;
		
		using iterator = pointer;
		using const_iterator = const_pointer;
		
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		
	public:
		/// MARK: Constructors
		/// (1)
		__utl_interface_export __utl_always_inline
		constexpr vector() noexcept(noexcept(Allocator()))
			requires std::is_default_constructible_v<Allocator>:
			vector({}, {}, 0, true) {}
		
		/// (2)
		__utl_interface_export __utl_always_inline
		constexpr explicit vector(Allocator const& alloc) noexcept:
			vector(alloc, {}, {}, 0, true) {}
		
		/// (3)
		__utl_interface_export __utl_always_inline
		constexpr vector(std::size_t count,
						 T const& value,
						 Allocator const& alloc = Allocator()):
			vector(alloc,
				   nullptr,
				   count,
				   count,
				   !count)
		{
			_set_begin(this->_allocate(count));
			for (auto i = begin(); i != end(); ++i) {
				_construct_at(i, value);
			}
		}
		
		/// (4)
		__utl_interface_export __utl_always_inline
		constexpr explicit vector(std::size_t count,
								  Allocator const& alloc = Allocator()):
			vector(alloc,
				   nullptr,
				   count,
				   count,
				   !count)
		{
			_set_begin(this->_allocate(count));
			for (auto i = begin(); i != end(); ++i) {
				_construct_at(i);
			}
		}
		
		/// (4a)
		__utl_interface_export __utl_always_inline
		constexpr explicit vector(std::size_t count,
								  utl::no_init_t,
								  Allocator const& alloc = Allocator())
			requires(std::is_trivial_v<T>):
			vector(alloc,
				   nullptr,
				   count,
				   count,
				   !count)
		{
			_set_begin(this->_allocate(count));
		}
		
		/// (5)
		template <input_iterator_for<T> InputIt>
		__utl_interface_export __utl_always_inline
		constexpr vector(InputIt first, InputIt last,
						 Allocator const& alloc = Allocator()):
			vector(alloc,
				   nullptr,
				   std::distance(first, last),
				   std::distance(first, last),
				   first == last)
		{
				_set_begin(this->_allocate(std::distance(first, last)));
				this->_copy_construct_range(first, last, this->begin());
		}
		
		/// (6)
		__utl_interface_export __utl_always_inline
		constexpr vector(vector const& rhs):
			vector(rhs._alloc(),
				   nullptr,
				   rhs.size(),
				   rhs.size(),
				   rhs.empty())
		{
			_set_begin(this->_allocate(rhs.size()));
			_move_construct_range(rhs._begin(), rhs._end(), this->_begin());
		}
		
		/// (6a)
		template <std::size_t N>
		__utl_interface_export __utl_always_inline
		vector(small_vector<T, N, Allocator> const& rhs):
			vector(rhs._alloc(),
				   nullptr,
				   rhs.size(),
				   rhs.size(),
				   rhs.empty())
		{
			_set_begin(this->_allocate(rhs.size()));
			_copy_construct_range(rhs._begin(), rhs._end(), this->_begin());
		}
		
		/// (7)
		__utl_interface_export __utl_always_inline
		constexpr vector(vector const& rhs, Allocator const& alloc):
			vector(rhs._alloc(),
				   nullptr,
				   rhs.size(),
				   rhs.size(),
				   rhs.empty())
		{
			_set_begin(this->_allocate(rhs.size()));
			_move_construct_range(rhs._begin, rhs._end, this->_begin);
		}
		
		/// (7a)
		template <std::size_t N>
		__utl_interface_export __utl_always_inline
		vector(small_vector<T, N, Allocator> const& rhs, Allocator const& alloc):
			vector(rhs._alloc(),
				   nullptr,
				   rhs.size(),
				   rhs.size(),
				   rhs.empty())
		{
			_set_begin(this->_allocate(rhs.size()));
			_copy_construct_range(rhs._begin, rhs._end, this->_begin);
		}
		
		/// (8)
		__utl_interface_export __utl_always_inline
		constexpr vector(vector&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>):
			vector(std::move(rhs._alloc()),
				   rhs.begin(),
				   rhs.size(),
				   rhs.size(),
				   rhs._uses_inline_buffer())
		{
			utl_expect(!rhs._uses_inline_buffer() || rhs.empty(), "error");
			rhs._set_begin(nullptr);
			rhs._set_size(0);
			rhs._set_cap(0);
			rhs._set_uses_inline_buffer(true);
		}
		
		/// (8a)
		template <std::size_t N>
		__utl_interface_export __utl_always_inline
		vector(small_vector<T, N, Allocator>&& rhs):
			Allocator(std::move(rhs._alloc()))
		{
			move_constr_impl(std::move(rhs));
		}
		
		/// (9)
		/// TODO: Fix this
		__utl_interface_export __utl_always_inline
		constexpr vector(vector&& rhs, Allocator const& alloc) noexcept(std::is_nothrow_move_constructible_v<T>) = delete;
//		constexpr vector(vector&& rhs, Allocator const& alloc) noexcept(std::is_nothrow_move_constructible_v<T>):
//			vector(std::move(rhs.alloc()),
//				   this->allocate(rhs.size()),
//				   rhs.size(),
//				   rhs.size(),
//				   rhs.empty())
//		{
//			utl_assert(!rhs._uses_inline_buffer(), "error");
//
//			move_construct_range(rhs._begin(), rhs._end(), this->begin());
//			rhs._set_begin(nullptr);
//			rhs._set_size(0);
//			rhs._set_cap(0);
//			rhs._set_uses_inline_buffer(true);
//		}
		
		/// (9a)
		/// TODO: Fix this
		template <std::size_t N>
		__utl_interface_export __utl_always_inline
		vector(small_vector<T, N, Allocator>&& rhs, Allocator const& alloc) = delete;
		
//		template <std::size_t N>
//		vector(small_vector<T, N, Allocator>&& rhs, Allocator const& alloc):
//			Allocator(std::move(alloc))
//		{
//			move_constr_impl(std::move(rhs));
//		}
		
		/// (10)
		__utl_interface_export __utl_always_inline
		constexpr vector(std::initializer_list<T> ilist,
						 Allocator const& alloc):
			vector(alloc,
				   nullptr,
				   ilist.size(),
				   ilist.size(),
				   false)
		{
			_set_begin(_allocate(ilist.size()));
			_copy_construct_range(ilist.begin(), ilist.end(), this->_begin());
		}
		
		/// (10a)
		__utl_interface_export __utl_always_inline
		constexpr vector(std::initializer_list<T> ilist)
		requires (std::is_default_constructible_v<Allocator>):
			vector(ilist, Allocator{})
		{}
		
		
		/// MARK: Destructor
		__utl_interface_export __utl_always_inline
		constexpr ~vector() {
			_destroy_elems();
			if (!this->_uses_inline_buffer()) {
				utl_assert(this->_begin() != nullptr, "must not be null");
				utl_assert(this->capacity() > 0, "must be positive");
				this->_deallocate(this->_begin(), this->capacity());
			}
		}
		
		
		/// MARK: operator=
		/// (1)
		__utl_interface_export
		constexpr vector& operator=(vector const& rhs)& {
			if UTL_UNLIKELY (&rhs == this) { return *this; }
			if (rhs.size() <= this->capacity()) {
				if constexpr (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
					// get the allocator
					this->_assign_alloc(rhs._alloc());
				}
				// no need to allocate
				auto const _rhs_mid_end = std::min(rhs.begin() + this->size(), rhs.end());
				// assign all we have already constructed in this
				std::copy(rhs.begin(), _rhs_mid_end, this->begin());
				// construct the rest
				_copy_construct_range(_rhs_mid_end, rhs.end(), this->end());
				
				auto const _old_end = this->end();
				this->_set_size(rhs.size());
				_destroy_elems(end(), _old_end);
			}
			else {
				_destroy_elems();
				_alloc().deallocate(this->_begin(), capacity());
				if constexpr (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
					// get the allocator
					this->_assign_alloc(rhs._alloc());
				}
				this->_set_begin(this->allocate(rhs.size()));
				this->_set_size(rhs.size());
				this->_set_cap(rhs.size());
				this->_set_uses_inline_buffer(false);
				_copy_construct_range(rhs._begin(), rhs._end(), this->_begin());
			}
			return *this;
		}
		/// (2)
		__utl_interface_export
		constexpr vector& operator=(vector&& rhs)& {
			if UTL_UNLIKELY (&rhs == this) { return *this; }
			if (rhs._uses_inline_buffer()) {
				this->_destroy_elems();

				if (rhs.size() <= capacity()) {
					this->_set_size(rhs.size());
				}
				else {
					this->_set_begin(this->allocate(rhs._size()));
					this->_set_size(rhs.size());
					this->_set_cap(rhs.size());
					this->_set_uses_inline_buffer(false);
				}

				_move_construct_range(rhs.begin(), rhs.end(), this->begin());

				rhs._destroy_elems();
				rhs._set_size(0);
			}
			else {
				_destroy_elems();
				if (!this->_uses_inline_buffer()) {
					this->deallocate(this->_begin(), this->_cap());
				}
				this->_set_begin(rhs._begin());
				this->_set_size(rhs._size());
				this->_set_cap(rhs._cap());
				this->_set_uses_inline_buffer(false);
				rhs._set_begin(nullptr);
				rhs._set_size(0);
				rhs._set_cap(0);
				rhs._set_uses_inline_buffer(true);
			}
			return *this;
		}
			
		/// (3)
		__utl_interface_export
		constexpr vector& operator=(std::initializer_list<T> ilist)& {
			if (ilist.size() <= this->capacity()) {
				// no need to allocate
				_copy_assign_range(ilist.begin(), ilist.begin() + this->_size(), this->_begin());
				_copy_construct_range(ilist.begin() + this->_size(), ilist.end(), this->_end());
				auto const old_end = this->end();
				this->_end = this->begin() + ilist.size();
				_destroy_elems(end(), old_end);
			}
			else {
				_destroy_elems();
				_alloc().deallocate(this->_begin(), this->capacity());
				
				this->_set_begin(this->allocate(ilist.size()));
				this->_set_size(ilist.size());
				this->_set_cap(ilist.size());
				this->_set_uses_inline_buffer(false);
				copy_construct_range(ilist.begin(), ilist.end(), this->begin());
			}
			return *this;
		}
		
		
		/// MARK: get_allocator
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr allocator_type get_allocator() const noexcept {
			return *this;
		}
		
		/// MARK: at
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T& at(std::size_t pos) {
			return as_mutable(utl::as_const(*this).at(pos));
		}
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T const& at(std::size_t pos) const {
			if (pos >= size()) {
				_throw_out_of_bound_error(pos);
			}
			return _begin()[pos];
		}
		
		
		/// MARK: operator[]
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T& operator[](std::size_t pos) {
			return utl::as_mutable(utl::as_const(*this).operator[](pos));
		}
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T const& operator[](std::size_t pos) const {
			utl_bounds_check(pos, 0, size());
			return _begin()[pos];
		}
		
		/// MARK: front
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T& front() noexcept { return as_mutable(utl::as_const(*this).front()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T const& front() const noexcept {
			utl_expect(!empty(), "front() called on empty vector.");
			return *_begin();
		}
		/// MARK: back
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T& back() noexcept { return as_mutable(utl::as_const(*this).back()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T const& back() const noexcept {
			utl_expect(!empty(), "back() called on empty vector.");
			return *(_end() - 1);
		}
		
		/// MARK: data
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T* data() noexcept { return as_mutable_ptr(utl::as_const(*this).data()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr T const* data() const noexcept {
			return _begin();
		}
		
		/// MARK: begin
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr iterator begin() noexcept { return _begin(); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_iterator begin() const noexcept { return _begin(); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_iterator cbegin() const noexcept { return _begin(); }
		
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(_end()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_reverse_iterator rbegin() const noexcept  { return reverse_Iterator(_end()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_reverse_iterator crbegin() const noexcept { return reverse_Iterator(_end()); }
		
		/// MARK: end
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr iterator end() noexcept { return _end(); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_iterator end() const noexcept { return _end(); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_iterator cend() const noexcept { return _end(); }
		
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr reverse_iterator rend() noexcept { return reverse_iterator(_begin()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_reverse_iterator rend() const noexcept  { return reverse_Iterator(_begin()); }
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr const_reverse_iterator crend() const noexcept { return reverse_Iterator(begin()); }
		
		/// MARK: empty
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr bool empty() const noexcept { return !size(); }
		
		/// MARK: size
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr std::size_t size() const noexcept { return _size(); }
		
		/// MARK: max_size
		__utl_nodiscard __utl_interface_export __utl_always_inline
		constexpr static std::size_t max_size() noexcept { return std::numeric_limits<size_type>::max(); }
		
		/// MARK: reserve
		__utl_interface_export
		constexpr void reserve(std::size_t new_cap) {
			if (new_cap <= capacity()) {
				return;
			}
			T* new_buffer = this->allocate(new_cap);
			_move_construct_range(this->_begin(), this->_end(), new_buffer);
			_destroy_elems();
			if (!_uses_inline_buffer()) {
				this->deallocate(this->_begin(), capacity());
			}
			auto const _size = size();
			this->_set_begin(new_buffer);
			this->_set_size(_size);
			this->_set_cap(new_cap);
			this->_set_uses_inline_buffer(false);
		}
		
		/// MARK: capacity
		__utl_nodiscard  __utl_interface_export __utl_always_inline
		constexpr std::size_t capacity() const noexcept { return this->_cap(); }
		
		/// MARK: shrink_to_fit
		__utl_interface_export __utl_always_inline
		constexpr void shrink_to_fit() {
			// no-op
		}
		
		/// MARK: clear
		__utl_interface_export __utl_always_inline
		constexpr void clear() noexcept {
			_destroy_elems();
			this->_set_size(0);
		}
		
		/// MARK: insert
		/// (1)
		__utl_interface_export __utl_always_inline
		constexpr iterator insert(const_iterator pos, T const& value) {
			return _insert_impl(pos, value);
		}
		
		/// (2)
		__utl_interface_export __utl_always_inline
		constexpr iterator insert(const_iterator pos, T&& value) {
			return _insert_impl(pos, std::move(value));
		}
		
		/// (3)
		__utl_interface_export __utl_always_inline
		constexpr iterator insert(const_iterator pos, size_type count,
								   T const& value);
		
		/// (4)
		template <typename InputIt>
		__utl_interface_export __utl_always_inline
		constexpr iterator insert(const_iterator pos,
								  InputIt first, InputIt last);
			
		/// (5)
		__utl_interface_export __utl_always_inline
		constexpr iterator insert(const_iterator pos,
								  std::initializer_list<T> ilist);
		
		
		/// MARK: push_back
		__utl_interface_export __utl_always_inline
		constexpr T& push_back(T const& t) { return emplace_back(t); }
		__utl_interface_export __utl_always_inline
		constexpr T& push_back(T&& t) { return emplace_back(std::move(t)); }
		
		/// MARK: emplace_back
		template <typename... Args>
		requires std::is_constructible_v<T, Args...>
		__utl_interface_export
		constexpr T& emplace_back(Args&&... args) {
			if UTL_UNLIKELY (size() == capacity()) {
				_grow();
			}
			_construct_at(_end(), std::forward<Args>(args)...);
			this->_set_size(this->_size() + 1);
			return back();
		}
	
		/// MARK: pop_back
		__utl_interface_export __utl_always_inline
		constexpr void pop_back() noexcept {
			utl_expect(!empty(), "pop_back() called on empty vector");
			this->_set_size(this->_size() - 1);
			_destroy_elem(_end());
		}
		
		/// MARK: erase
		/// (1)
		__utl_interface_export
		constexpr iterator erase(const_iterator cpos) {
			utl_bounds_check(cpos, begin(), end());
			iterator pos = const_cast<iterator>(cpos);
			this->_left_shift_range(pos + 1, end(), -1);
			_destroy_elems(end() - 1, end());
			_set_size(size() - 1);
			return pos;
		}
		/// (1a)
		__utl_interface_export __utl_always_inline
		constexpr void erase(std::size_t index) {
			erase(begin() + index);
		}
		/// (2)
		__utl_interface_export __utl_always_inline
		constexpr iterator erase(const_iterator first, const_iterator last);
		
		/// MARK: resize
		/// (1)
		__utl_interface_export __utl_always_inline
		constexpr void resize(std::size_t count) {
			resize_impl(count, [&](auto i) { _construct_at(i); });
		}
		/// (2)
		__utl_interface_export __utl_always_inline
		constexpr void resize(std::size_t count, T const& value) {
			resize_impl(count, [&](auto i) { _construct_at(i, value); });
		}
		/// (3)
		__utl_interface_export __utl_always_inline
		constexpr void resize(std::size_t count, no_init_t) {
			resize_impl(count, [](auto) {});
		}
		
		/// MARK: swap
		__utl_interface_export __utl_always_inline
		constexpr void swap(vector& other) noexcept;
		
		
		
		
		/// MARK: - Private Interface
	private:
		template <typename, std::size_t, typename>
		friend class small_vector;
		
		template <std::size_t N>
		void move_constr_impl(small_vector<T, N, Allocator>&& rhs) {
			if (rhs._uses_inline_buffer()) {
				this->_set_begin(this->_allocate(rhs.size()));
				this->_set_size(rhs.size());
				this->_set_cap(rhs.size());
				this->_set_uses_inline_buffer(rhs.empty());
				_move_construct_range(rhs._begin(), rhs._end(), this->_begin());
				rhs._destroy_elems();
				rhs._set_size(0);
			}
			else {
				this->_set_begin(rhs._begin());
				this->_set_size(rhs.size());
				this->_set_cap(rhs._cap());
				this->_set_uses_inline_buffer(rhs._uses_inline_buffer());
				
				rhs._set_begin(rhs._storage_begin());
				rhs._set_size(0);
				rhs._set_cap(N);
				rhs._set_uses_inline_buffer(true);
			}
		}
		
		template <typename U>
		constexpr iterator _insert_impl(const_iterator cpos, U&& value) {
			if (cpos == end()) {
				push_back(std::forward<U>(value));
				return end() - 1;
			}
			else {
				utl_bounds_check(cpos, begin(), end());
				std::size_t const index = cpos - begin();
				if (size() == capacity()) {
					reserve(_recommend(size() + 1));
				}
				auto pos = begin() + index;
				_construct_at(end(), std::move(back()));
				_right_shift_range(pos, end(), 1);
				_set_size(size() + 1);
				*pos = std::forward<U>(value);
				return pos;
			}
		}
		
		constexpr void resize_impl(std::size_t count, auto&& constr) {
			if (size() == count) {
				return;
			}
			else if (size() > count) {
				_destroy_elems(begin() + count, end());
				this->_set_size(count);
			}
			else /* we grow */ {
				auto const old_size = size();
				if (count > capacity()) {
					auto const target_cap = _recommend(count);
					auto const new_buffer = this->allocate(target_cap);
					_move_construct_range(begin(), end(), new_buffer);
					_destroy_elems();
					if (!_uses_inline_buffer()) {
						this->deallocate(_begin(), capacity());
					}
					this->_set_begin(new_buffer);
					this->_set_size(count);
					this->_set_cap(target_cap);
					this->_set_uses_inline_buffer(false);
				}
				else {
					this->_set_size(count);
				}
				for (auto i = _begin() + old_size, e = _end(); i < e; ++i) {
					constr(i);
				}
			}
		}
		
		constexpr std::size_t _recommend(std::size_t new_size) noexcept {
			using conf = __vector_config<T, Allocator>;
			new_size = std::max(new_size, conf::minimum_allocation_count);
			constexpr std::size_t ms = max_size();
			utl_assert(new_size <= ms, "max_size exceeded");
			auto const cap = capacity();
			auto const next_cap = cap * conf::growth_factor;
			if UTL_UNLIKELY (next_cap >= ms)
				return ms;
			return std::bit_ceil(std::max<std::size_t>(next_cap, new_size));
		}
		
		explicit constexpr vector(Allocator const& alloc, no_init_t):
			Allocator(alloc) {}
		
		constexpr Allocator& _alloc() noexcept { return static_cast<Allocator&>(*this); }
		constexpr Allocator const& _alloc() const noexcept { return static_cast<Allocator const&>(*this); }
		
		constexpr T* _allocate(std::size_t n) {
			return n != 0 ? _alloc().allocate(n) : nullptr;
		}
		
		constexpr void _deallocate(T* ptr, std::size_t n) {
			if (ptr) {
				_alloc().deallocate(ptr, n);
			}
		}

		template <typename... Args>
		constexpr void _construct_at(T* address, Args&&... args) {
			std::allocator_traits<Allocator>::construct(*this, address, UTL_FORWARD(args)...);
		}
		constexpr void _destroy_at(T* address) {
			std::allocator_traits<Allocator>::destroy(*this, address);
		}
		
		constexpr void _grow() {
			utl_expect(capacity() == size(), "no need to grow");
			reserve(_recommend(size() + 1));
		}
		
		constexpr void _destroy_elems() { _destroy_elems(begin(), end()); }
		constexpr void _destroy_elems(T* begin, T* end) {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (auto i = begin; i < end; ++i) {
					_destroy_at(i);
				}
			}
		}
		
		template <typename Iter>
		constexpr void _copy_construct_range(Iter begin, Iter end, T* out) noexcept(std::is_nothrow_copy_constructible_v<T>) {
			for (auto i = begin; i != end; ++i, ++out) {
				_construct_at(out, static_cast<T>(*i));
			}
		}
		template <typename Iter>
		constexpr void _copy_assign_range(Iter begin, Iter end, T* out) noexcept(std::is_nothrow_copy_constructible_v<T>) {
			for (auto i = begin; i != end; ++i, ++out) {
				*out = *i;
			}
		}
		
		template <typename Iter>
		constexpr void _move_construct_range(Iter begin, Iter end, T* out) noexcept(std::is_nothrow_move_constructible_v<T>) {
			for (auto i = begin; i != end; ++i, ++out) {
				_construct_at(out, std::move(*i));
			}
		}
		
		template <typename Iter>
		constexpr void _move_assign_range(Iter begin, Iter end, T* out) noexcept(std::is_nothrow_move_assignable_v<T>) {
			for (auto i = begin; i != end; ++i, ++out) {
				*out = std::move(*i);
			}
		}
		
		constexpr static void _left_shift_range(T* begin, T* end, std::ptrdiff_t offset) noexcept(std::is_nothrow_move_assignable_v<T>) {
			utl_expect(offset < 0, "offset must be negative");
			for (auto i = begin + offset, j = begin; j != end; ++i, ++j) {
				*i = std::move(*j);
			}
		}
		
		constexpr static void _right_shift_range(T* begin, T* end, std::ptrdiff_t offset) noexcept(std::is_nothrow_move_assignable_v<T>) {
			utl_expect(offset > 0, "offset must be positive");
			for (auto i = end + offset - 1, j = end - 1; j >= begin; --i, --j) {
				*i = std::move(*j);
			}
		}
		
		__utl_noinline
		void _throw_out_of_bound_error(size_type pos) const {
			throw std::out_of_range("utl::vector out of bounds");
		}
		
		constexpr vector(T* begin, std::size_t size, std::size_t capacity, bool inplace) noexcept:
			Allocator(), _begin_inline_(begin, inplace), _size_((size_type)size), _cap_(static_cast<size_type>(capacity)) {}
		
		constexpr vector(auto&& alloc, T* begin, std::size_t size, std::size_t capacity, bool inplace) noexcept:
			Allocator(UTL_FORWARD(alloc)), _begin_inline_(begin, inplace), _size_((size_type)size), _cap_((size_type)capacity) {}
		
		constexpr pointer _begin() const noexcept { return _begin_inline_.pointer(); }
		constexpr void _set_begin(pointer p) noexcept { _begin_inline_.pointer(p); }
		constexpr auto _size() const noexcept { return _size_; }
		constexpr void _set_size(std::size_t size) noexcept {
			_size_ = narrow_cast<size_type>(size);
		}
		constexpr auto _cap() const noexcept { return _cap_; }
		constexpr void _set_cap(std::size_t cap) noexcept {
			_cap_ = narrow_cast<size_type>(cap);
		}
		
		constexpr bool _uses_inline_buffer() const noexcept {
			return _begin_inline_.integer();
		}
		
		constexpr void _set_uses_inline_buffer(bool b) noexcept { _begin_inline_.integer(b); }
		constexpr pointer _end() const noexcept { return _begin() + _size_; }
		
	private:
		pointer_int_pair<T*, bool, 1> _begin_inline_;
		size_type _size_, _cap_;
		
	private:
		pointer _storage_begin() noexcept { return nullptr; }
		pointer _storage_end() noexcept { return nullptr; }
		
		const_pointer _storage_begin() const noexcept { return nullptr; }
		const_pointer _storage_end() const noexcept { return nullptr; }
	};
	
	
	/// MARK: operator<<
	template <typename T, typename A>
	std::ostream& operator<<(std::ostream& _str, utl::vector<T, A> const& v) {
		auto& str = [](auto& x) -> decltype(auto) { return x; }(_str);
		str << "[";
		bool first = true;
		for (auto const& i: v) {
			str << (first ? ((void)(first = false), "") : ", ") << i;
		}
		return str << "]";
	}
	
	
	/// MARK: - Free Member Function Alternatives
	template <typename T, typename A>
	constexpr auto begin(vector<T, A>& v) { return v.begin(); }
	template <typename T, typename A>
	constexpr auto begin(vector<T, A> const& v) { return v.begin(); }
	template <typename T, typename A>
	constexpr auto begin(vector<T, A>&& v) { return std::move(v).begin(); }
	template <typename T, typename A>
	constexpr auto begin(vector<T, A> const&& v) { return std::move(v).begin(); }
	
	template <typename T, typename A>
	constexpr auto end(vector<T, A>& v) { return v.end(); }
	template <typename T, typename A>
	constexpr auto end(vector<T, A> const& v) { return v.end(); }
	template <typename T, typename A>
	constexpr auto end(vector<T, A>&& v) { return std::move(v).end(); }
	template <typename T, typename A>
	constexpr auto end(vector<T, A> const&& v) { return std::move(v).end(); }
	
	template <typename T, typename A>
	constexpr auto size(vector<T, A> const& v) { return v.size(); }
	
	template <typename T, typename A>
	constexpr auto* data(vector<T, A>& v) { return v.data(); }
	template <typename T, typename A>
	constexpr auto const* data(vector<T, A> const& v) { return v.data(); }
	
	
	
	
	/// MARK: struct __vector_config
	template <typename T, typename A>
	struct __vector_config {
		static constexpr std::size_t growth_factor = 2;
		
		static constexpr std::size_t minimum_allocation_count = 4; // default_inline_capacity<T, A>;
		
		static constexpr std::size_t small_vector_target_size = 64;
		
		using size_type = typename vector_size_type_selector<T>::type;
	};

	template <typename T, typename A>
	constexpr std::size_t __small_vector_default_inline_capacity() {
		auto constexpr base_vector_size = sizeof(utl::vector<T, A>);
		std::size_t const target_count = (__vector_config<T, A>::small_vector_target_size - base_vector_size) / sizeof(T);
		return std::max<std::size_t>(target_count, 0);
	}

	
	
	
	
	
	
	
	
	/// MARK: - class small_vector
	template <typename T, std::size_t N, typename Allocator>
	class small_vector: public vector<T, Allocator> {
		using __base = vector<T, Allocator>;
				
	public:
		/// MARK: Member Types
		using typename __base::value_type;
		using typename __base::allocator_type;
		using typename __base::size_type;
		using typename __base::difference_type;
		
		using typename __base::reference;
		using typename __base::const_reference;
		using typename __base::pointer;
		using typename __base::const_pointer;
		
		using typename __base::iterator;
		using typename __base::const_iterator;
		
		using typename __base::reverse_iterator;
		using typename __base::const_reverse_iterator;
		
	public:
		/// MARK: Constructors
		/// (1)
		__utl_interface_export __utl_always_inline
		constexpr small_vector() noexcept(noexcept(Allocator()))
		requires std::is_default_constructible_v<Allocator>:
			__base(_storage_begin(), {}, N, true) {}
		
		/// (2)
		__utl_interface_export __utl_always_inline
		constexpr explicit small_vector(Allocator const& alloc) noexcept:
			__base(alloc, _storage_begin(), {}, N, true) {}
		
		/// (3)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(std::size_t count,
							   T const& value,
							   Allocator const& alloc = Allocator()):
			__base(alloc, no_init)
		{
			_count_constructor_prep(count);
			for (auto i = this->begin(); i != this->end(); ++i) {
				this->_construct_at(i, value);
			}
		}
		
		/// (4)
		__utl_interface_export __utl_always_inline
		constexpr explicit small_vector(std::size_t count,
										Allocator const& alloc = Allocator()):
			__base(alloc, no_init)
		{
			_count_constructor_prep(count);
			for (auto i = this->begin(); i != this->end(); ++i) {
				this->_construct_at(i);
			}
		}
		
		/// (4a)
		__utl_interface_export __utl_always_inline
		constexpr explicit small_vector(std::size_t count,
											utl::no_init_t,
											Allocator const& alloc = Allocator())
			requires(std::is_trivial_v<T>):
			__base(alloc, no_init)
		{
			_count_constructor_prep(count);
		}
		
		/// (5)
		template <typename InputIt>
		__utl_interface_export __utl_always_inline
		constexpr small_vector(InputIt first, InputIt last,
							   Allocator const& alloc = Allocator())
		requires requires{ { *first } -> convertible_to<T>; }:
			__base(alloc, no_init)
		{
			_count_constructor_prep(std::distance(first, last));
			this->_copy_construct_range(first, last, this->begin());
		}
		
		/// (6)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector const& rhs):
			__base(rhs._alloc(), no_init)
		{
			_copy_constructor_impl(rhs);
		}
		/// (6a)
		template <std::size_t M>
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector<T, M, Allocator> const& rhs):
			__base(rhs._alloc(), no_init)
		{
			_copy_constructor_impl(rhs);
		}
		
		/// (6b)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(vector<T, Allocator> const& rhs):
			__base(rhs._alloc(), no_init)
		{
			_copy_constructor_impl(rhs);
		}
		
		/// (7)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector const& rhs, Allocator const& alloc):
			__base(alloc, no_init)
		{
			_copy_constructor_impl(rhs);
		}
		/// (7a)
		template <std::size_t M>
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector<T, M, Allocator> const& rhs, Allocator const& alloc):
			__base(alloc, no_init)
		{
			_copy_constructor_impl(rhs);
		}
		
		/// (7b)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(vector<T, Allocator> const& rhs, Allocator const& alloc):
			__base(alloc, no_init)
		{
			_copy_constructor_impl(rhs);
		}
		
		/// (8)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector&& rhs) noexcept:
			__base(std::move(rhs._alloc()), no_init)
		{
			_move_constructor_impl<0>(std::move(rhs));
		}
		/// (8a)
		template <std::size_t M>
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector<T, M, Allocator>&& rhs) noexcept:
			__base(std::move(rhs._alloc()), no_init)
		{
			_move_constructor_impl<M>(std::move(rhs));
		}
		/// (8b)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(vector<T, Allocator>&& rhs) noexcept:
			__base(std::move(rhs._alloc()), no_init)
		{
			_move_constructor_impl<0>(std::move(rhs));
		}
		
		/// (9)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector&& rhs, Allocator const& alloc) noexcept:
			__base(alloc, no_init)
		{
			_move_constructor_impl<N>(std::move(rhs));
		}
		/// (9a)
		template <std::size_t M>
		__utl_interface_export __utl_always_inline
		constexpr small_vector(small_vector<T, M, Allocator>&& rhs, Allocator const& alloc) noexcept:
			__base(alloc, no_init)
		{
			_move_constructor_impl(std::move(rhs));
		}
		/// (9b)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(vector<T, Allocator>&& rhs, Allocator const& alloc) noexcept:
			__base(alloc, no_init)
		{
			_move_constructor_impl(std::move(rhs));
		}
		
		/// (10)
		__utl_interface_export __utl_always_inline
		constexpr small_vector(std::initializer_list<T> ilist,
							   Allocator const& alloc = Allocator()):
			__base(alloc, no_init)
		{
			_count_constructor_prep(ilist.size());
			this->_copy_construct_range(ilist.begin(), ilist.end(), this->_begin());
		}
		
		/// MARK: operator=
		using __base::operator=;
		/// (1)
		__utl_interface_export __utl_always_inline
		constexpr small_vector& operator=(small_vector const& other)& {
			static_cast<__base&>(*this) = static_cast<__base const&>(other);
			return *this;
		}
		/// (2)
		__utl_interface_export __utl_always_inline
		constexpr small_vector& operator=(small_vector&& other)& noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value ||
																		  std::allocator_traits<Allocator>::is_always_equal::value)
		{
			static_cast<__base&>(*this) = static_cast<__base&&>(other);
			return *this;
		}
		/// (3)
		__utl_interface_export __utl_always_inline
		constexpr small_vector& operator=(std::initializer_list<T> ilist)& {
			static_cast<__base&>(*this) = ilist;
			return *this;
		}
		
		/// MARK: inline_capacity
		__utl_interface_export __utl_always_inline
		constexpr static std::size_t inline_capacity() noexcept { return N; }
		
		
		
		
		/// MARK: - Private Interface
	private:
		friend class vector<T, Allocator>;
		template <typename, std::size_t, typename>
		friend class small_vector;
		
		constexpr void _count_constructor_prep(std::size_t count) {
			if (count <= N) {
				this->_set_begin(this->_storage_begin());
				this->_set_size(count);
				this->_set_cap(N);
				this->_set_uses_inline_buffer(true);
			}
			else {
				this->_set_begin(this->_allocate(count));
				this->_set_size(count);
				this->_set_cap(count);
				this->_set_uses_inline_buffer(false);
			}
		}
		
		void _copy_constructor_impl(auto const& rhs) {
			if (rhs.size() <= N) {
				this->_set_begin(this->_storage_begin());
				this->_set_size(rhs.size());
				this->_set_cap(N);
				this->_set_uses_inline_buffer(true);
			}
			else {
				this->_set_begin(this->_allocate(rhs.size()));
				this->_set_size(rhs.size());
				this->_set_cap(rhs.size());
				this->_set_uses_inline_buffer(false);
			}
			this->_copy_construct_range(rhs.begin(), rhs.end(), this->begin());
		}
		
		template <std::size_t rhsCap>
		void _move_constructor_impl(auto&& rhs) {
			if (rhs.size() <= N) /* contents of rhs fits into our local buffer */ {
				this->_set_begin(this->_storage_begin());
				this->_set_size(rhs.size());
				this->_set_cap(N);
				this->_set_uses_inline_buffer(true);
				this->_move_construct_range(rhs._begin(), rhs._end(), this->_begin());
				rhs._destroy_elems();
				rhs._set_size(0);
			}
			else if (rhs._uses_inline_buffer()) /* rhs uses local buffer, we need to allocate */ {
				this->_set_begin(this->_allocate(rhs.size()));
				this->_set_size(rhs.size());
				this->_set_cap(rhs.size());
				this->_set_uses_inline_buffer(rhs.empty());
				this->_move_construct_range(rhs._begin(), rhs._end(), this->_begin());
				rhs._destroy_elems();
				rhs._set_size(0);
			}
			else /* we can swap buffers */ {
				this->_set_begin(rhs._begin());
				this->_set_size(rhs._size());
				this->_set_cap(rhs._cap());
				this->_set_uses_inline_buffer(rhs._uses_inline_buffer());
				rhs._set_begin(rhs._storage_begin());
				rhs._set_size(0);
				rhs._set_cap(rhsCap);
				rhs._set_uses_inline_buffer(true);
			}
		}
		
	private:
		pointer _storage_begin() noexcept { return reinterpret_cast<pointer>(_storage); }
		pointer _storage_end() noexcept { return reinterpret_cast<pointer>(_storage) + N; }
		
		const_pointer _storage_begin() const noexcept { return reinterpret_cast<const_pointer>(_storage); }
		const_pointer _storage_end() const noexcept { return reinterpret_cast<const_pointer>(_storage) + N; }
		
		__base& _as_base() noexcept { return static_cast<__base&>(*this); }
		__base const& _as_base() const noexcept { return static_cast<__base const&>(*this); }
		
		alignas (N != 0 ? alignof(T) : 1) char _storage[sizeof(T) * N];
	};
	
}





namespace utl {
	
	// MARK: - weird set operatioms
	template <typename T, typename A>
	bool is_set(utl::vector<T, A> a) {
		return std::unique(a.begin(), a.end()) == a.end();
	}
	
	template <typename T, typename A>
	bool is_sorted(utl::vector<T, A> const& a) {
		return std::is_sorted(a.begin(), a.end());
	}
	
	template <typename T, typename A>
	vector<T, A> set_union(utl::vector<T, A> const& a, utl::vector<T, A> const& b) {
		utl_assert(is_set(a), "a must be unique");
		utl_assert(is_set(b), "b must be unique");
		utl_assert(is_sorted(a), "a must be sorted");
		utl_assert(is_sorted(b), "b must be sorted");
		utl::vector<T, A> result;
		
		std::set_union(a.begin(), a.end(),
					   b.begin(), b.end(),
					   std::inserter(result, result.end()));
		return result;
	}
	
	template <typename T, typename A>
	utl::vector<T, A> set_difference(utl::vector<T, A> const& a, utl::vector<T, A> const& b) {
		utl_assert(is_set(a), "a must be unique");
		utl_assert(is_set(b), "b must be unique");
		utl_assert(is_sorted(a), "a must be sorted");
		utl_assert(is_sorted(b), "b must be sorted");
		utl::vector<T, A> result;
		
		std::set_difference(a.begin(), a.end(),
							b.begin(), b.end(),
							std::inserter(result, result.end()));
		return result;
	}
	
	template <typename T, typename A>
	utl::vector<T, A> set_intersection(utl::vector<T, A> const& a, utl::vector<T, A> const& b) {
		utl_assert(is_set(a), "a must be unique");
		utl_assert(is_set(b), "b must be unique");
		utl_assert(is_sorted(a), "a must be sorted");
		utl_assert(is_sorted(b), "b must be sorted");
		utl::vector<T, A> result;
		
		std::set_intersection(a.begin(), a.end(),
							  b.begin(), b.end(),
							  std::inserter(result, result.end()));
		return result;
	}
	
	template <typename T, typename A>
	utl::vector<T, A> set_symmetric_difference(utl::vector<T, A> const& a, utl::vector<T, A> const& b) {
		utl_assert(is_set(a), "a must be unique");
		utl_assert(is_set(b), "b must be unique");
		utl_assert(is_sorted(a), "a must be sorted");
		utl_assert(is_sorted(b), "b must be sorted");
		vector<T, A> result;
		
		std::set_symmetric_difference(a.begin(), a.end(),
									  b.begin(), b.end(),
									  std::inserter(result, result.end()));
		return result;
	}
	
}


