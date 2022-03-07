#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "type_traits.hpp"
#include <type_traits>
#include "__memory_resource_base.hpp"
#include <memory>

namespace utl {
	
	template <typename T>
	using ref = std::shared_ptr<T>;
	
	template <typename T, typename ... Args, UTL_ENABLE_IF(std::is_constructible<T, Args...>::value)>
	ref<T> make_ref(Args&&... args) {
		return std::allocate_shared<T>(pmr::polymorphic_allocator<T>(), std::forward<Args>(args)...);
	}
	
	template <typename T>
	ref<utl::remove_cvref_t<T>> make_ref(T&& t) {
		using U = utl::remove_cvref_t<T>;
		return std::allocate_shared<U>(pmr::polymorphic_allocator<U>(), std::forward<T>(t));
	}
	
	template <typename T>
	struct enable_ref_from_this: std::enable_shared_from_this<T> {
		utl::ref<T> ref() { return this->shared_from_this(); }
		utl::ref<T const> ref() const { return this->shared_from_this(); }
	};
	
//	template <typename T>
//	struct pmr_delete {
//		static_assert(!is_function<T>::value,
//					  "default_delete cannot be instantiated for function types");
//	
//		constexpr default_delete() noexcept = default;
//	
//		template <typename U, UTL_ENABLE_IF(is_convertible<U*, T*>::value)>
//		default_delete(default_delete<U> const&) noexcept {}
//
//		void operator()(T* ptr) const noexcept {
//			static_assert(sizeof(T) > 0, "default_delete can not delete incomplete type");
//			static_assert(!is_void<T>::value, "default_delete can not delete incomplete type");
//			_resource->deallocate(ptr, <#std::size_t size#>, <#std::size_t alignment#>)
//	  }
//		
//	private:
//		memory_resource* _resource;
//	};
	
	template <typename T, typename Deleter = std::default_delete<T>>
	using unique_ref = std::unique_ptr<T, Deleter>;
	
	template <typename T, typename ... Args>
	unique_ref<T> make_unique_ref(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }
	
	template <typename T>
	using weak_ref = std::weak_ptr<T>;
	
}
