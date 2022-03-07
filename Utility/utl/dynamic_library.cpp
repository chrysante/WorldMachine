#include "dynamic_library.hpp"

#include <dlfcn.h>
#include "strcat.hpp"
#include "debug.hpp"
#include "scope_guard.hpp"
#include <exception>

namespace utl {
	
	dynamic_library::dynamic_library(std::filesystem::path path): _path(path) {
		_load_lib();
	}
	
	dynamic_library::dynamic_library(dynamic_library&& rhs) noexcept:
		_path(std::move(rhs._path)),
		_handle(rhs._handle)
	{
		rhs._handle = nullptr;
	}
	
	dynamic_library::~dynamic_library() {
		_destroy();
	}
	
	dynamic_library& dynamic_library::operator=(dynamic_library&& rhs)& noexcept {
		_destroy();
		this->_path = std::move(rhs._path);
		this->_handle = rhs._handle;
		rhs._handle = nullptr;
		return *this;
	}
	
	void dynamic_library::reload() {
		_destroy();
		_load_lib();
	}
	
	void dynamic_library::load(std::filesystem::path new_path) {
		_path = new_path;
		reload();
	}
	
	bool dynamic_library::is_loaded() const {
		void* handle = dlopen(_path.c_str(), RTLD_NOW | RTLD_NOLOAD);
		if (handle != nullptr) {
		  dlclose(handle);
		  return true;
		}
		return false;
	}
	
	// MARK: - private
	void* dynamic_library::_load_symbol(std::string_view name) const {
		_clear_errors();
		auto* const result = dlsym(_handle, name.data());
		_handle_error(utl::strcat("Failed to load Symbol \"", name, "\""));
		return result;
	}
	
	void dynamic_library::_load_lib() {
		utl_assert(!_path.empty(), "path is empty");
		_clear_errors();
		_handle = dlopen(_path.c_str(), RTLD_NOW);
		utl::scope_guard_failure const _on_failure = [&]{
			_handle = nullptr;
		};
		_handle_error("Failed to load Library");
	}
	
	void dynamic_library::_destroy() noexcept {
		utl_guard (_handle) else { return; }
		[[maybe_unused]] auto const result = dlclose(_handle);
		utl_assert(result == 0, "Failed to close Library");
		
		dlopen(_path.c_str(), RTLD_NOLOAD);
		
		if (is_loaded()) {
			utl_log(error, "Failed to unload library {}", _path);
		}
		else {
			utl_log(trace, "Successfully unloaded library {}", _path);
		}
		_handle = nullptr;
	}
	
	void dynamic_library::_handle_error(std::string_view msg) const {
		if (char const* error = dlerror()) {
			throw std::runtime_error(utl::strcat(msg, ". Native Error:\n", error));
		}
	}
	
	void dynamic_library::_clear_errors() const {
		dlerror();
	}
	
}
