#include "memory_resource.hpp"

#include "stdio.hpp"

namespace utl::pmr {

	
	_private::_new_delete_resource* _private::_get_new_delete_resource() noexcept {
		static _new_delete_resource resource;
		return &resource;
	}
	
	_private::_null_memory_resource* _private::_get_null_memory_resource() noexcept {
		static _null_memory_resource resource;
		return &resource;
	}
	
	memory_resource* _private::_default_resource_manager(bool set, memory_resource* other) {
		static memory_resource* default_resource = utl::pmr::new_delete_resource();
		if UTL_UNLIKELY (set) {
			default_resource = other;
			return nullptr;
		}
		return default_resource;
	}
	
	void* monitor_resource::do_allocate(std::size_t size, std::size_t align) {
		auto const result = upstream()->allocate(size, align);
		utl::print("allocate(size: {}, align: {}) -> {}\n", size, align, result);
		return result;
	}

	void monitor_resource::do_deallocate(void* ptr, std::size_t size, std::size_t align) {
		utl::print("deallocate(ptr: {}, size: {}, align: {})\n", ptr, size, align);
		upstream()->deallocate(ptr, size, align);
	}

}
