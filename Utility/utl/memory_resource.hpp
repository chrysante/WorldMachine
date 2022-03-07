#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

#include "__memory_resource_base.hpp"

#include "debug.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
//#include "small_vector.hpp"
#include <cstddef>
#include <new>
#include <memory>
#include <exception>

// MARK: - monotonic_buffer_resource
namespace utl::pmr {
	
	class monotonic_buffer_resource: public memory_resource {
		static constexpr std::size_t default_size = 1024;
		
	public:
		// MARK: constructor
		/*
		 1-2) Sets the current buffer to null and the next buffer size to an implementation-defined size.
		 3-4) Sets the current buffer to null and the next buffer size to a size no smaller than initial_size.
		 5-6) Sets the current buffer to buffer and the next buffer size to buffer_size (but not less than 1). Then increase the next buffer size by an implementation-defined growth factor (which does not have to be integral).
		 7) Copy constructor is deleted.
		 */
		// (1)
//		monotonic_buffer_resource():
//			monotonic_buffer_resource(get_default_resource())
//		{ throw; }
		// (2)
//		explicit monotonic_buffer_resource(memory_resource* upstream):
//			_upstream(upstream),
//			_localBuffer(nullptr),
//			_localBufferSize(0)
//			_bufferList(nullptr),
//			_nextBufferSize(default_size)
//		{ throw; }
		// (3)
//		explicit monotonic_buffer_resource(std::size_t initial_size):
//			monotonic_buffer_resource(initial_size, get_default_resource())
//		{ throw; }
			
		// (4)
//		monotonic_buffer_resource(std::size_t initial_size,
//								  memory_resource* upstream):
//			_upstream(upstream),
//			_localBuffer({  }),
//			_bufferList(nullptr),
//			_nextBufferSize(initial_size)
//		{ throw; }
		// (5)
		monotonic_buffer_resource(void* buffer, std::size_t buffer_size):
			monotonic_buffer_resource(buffer, buffer_size, get_default_resource())
		{  }
		// (6)
		monotonic_buffer_resource(void* buffer, std::size_t buffer_size,
								  memory_resource* upstream):
			_upstream(upstream),
			_localBuffer({ (char*)buffer, (char*)buffer, (char*)buffer + buffer_size })
//			_bufferList(nullptr),
//			_nextBufferSize(_calculateNextBufferSize(buffer_size))
		{  }
		
		// (7)
		monotonic_buffer_resource(const monotonic_buffer_resource&) = delete;
		
		
		// MARK: destructor
		~monotonic_buffer_resource() { release(); }
		// MARK: release
		/*
		 Releases all allocated memory by calling the deallocate function on the upstream memory resource as necessary. Resets current buffer and next buffer size to their initial values at construction.

		 Memory is released back to the upstream resource even if deallocate has not been called for some of the allocated blocks.
		 */
		void release() noexcept {
//			for (auto i = _bufferList; i != nullptr; i = i->prev()) {
//				upstream_resource()->deallocate(i, i->total_buffer_size(),
//												_alignment);
//			}
		}
		
		// MARK: upstream_resource
		memory_resource* upstream_resource() const { return _upstream; }
		
		// MARK: do_allocate
		/*
		 Allocates storage.

		 If the current buffer has sufficient unused space to fit a block with the specified size and alignment, allocates the return block from the current buffer.

		 Otherwise, this function allocates a new buffer by calling upstream_resource()->allocate(n, m), where n is not less than the greater of bytes and the next buffer size and m is not less than alignment. It sets the new buffer as the current buffer, increases the next buffer size by an implementation-defined growth factor (which is not necessarily integral), and then allocates the return block from the newly allocated buffer.
		 */
		void* do_allocate(std::size_t bytes, std::size_t alignment) override {
			auto const result = utl::align_to(_localBuffer.current, alignment);
			if (_localBuffer.end - result < bytes) {
				throw std::bad_alloc{};
			}
//			_localBuffer.current = result + bytes;
//			return result;
//			auto const currentBuffer =
//				_localBuffer.begin ?
//				_localBuffer.begin :
//				_bufferList ?
//				_bufferList->current() :
//				_allocateNewBuffe(bytes, alignment)
//
//
//
//			void* candidate = align_to(_currentBuffer.current(), alignment);
//			if (utl::pointer_distance(candidate, _currentBuffer.end()) >= bytes) {
//				return candidate;
//			}
//
//
//
//			_currentBuffer
			
			return nullptr;
		}
		
		// MARK: do_deallocate
		/*
		 no-op
		 */
		void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
			
		}

		// MARK: do_is_equal
		/*
		 Checks if other is-a monotonic_buffer_resource - memory allocated using a monotonic_buffer_resource can only be deallocated using any monotonic_buffer_resource.
		 */
		bool do_is_equal(memory_resource const& other) const noexcept override {
			return dynamic_cast<monotonic_buffer_resource const*>(&other) != nullptr;
		}
				

	private:
		char* _currentBuffer(std::size_t size_for, std::size_t align_for) const {
			if (_usesLocalBuffer()) {
				return _localBuffer.current;
			}
			if (_bufferList) {
				return _bufferList->current();
			}
			return nullptr;
		}
		
		char const* _currentBufferEnd() const {
			if (_usesLocalBuffer()) {
				return _localBuffer.end;
			}
			utl_assert(_bufferList, "make sure _currentBuffer() has been called before");
			return _bufferList->end();
		}
		
		bool _usesLocalBuffer() const {
			return !_bufferList && _localBuffer.begin;
		}
		
//		static std::size_t _calculateNextBufferSize(std::size_t lastBufferSize) {
//			return lastBufferSize * 2;
//		}
		
		struct BufferNodeHeader {
			BufferNodeHeader(char const* end, BufferNodeHeader* prev):
				_current(_begin()),
				_end(end),
				_prev(prev)
			{}


			char* current() const { return _current; }
			char const* end() const { return _end; }
			BufferNodeHeader* prev() const { return _prev; }

			std::size_t total_buffer_size() const {
				return end() - (char*)this;
			}

			std::size_t free_space() const {
				return end() - current();
			}

		private:
			char* _begin() const;

		private:
			char* _current;
			char const* _end;
			BufferNodeHeader* _prev;
		};
		
		static constexpr std::size_t _bufferAlignment = std::bit_ceil(sizeof(BufferNodeHeader));
		
		char* _allocateNewBufferFor(std::size_t bytes, std::size_t alignment) {
			std::size_t const size = std::max(bytes, _nextBufferSize) + _bufferAlignment;
			_nextBufferSize *= 2;
			
		}
		
		
		
//		monotonic_buffer_resource(memory_resource* upstream, , std::size_t nextSize):
//			_upstream(upstream),
//			_currentBuffer(current),
//			_nextBufferSize(nextSize)
//		{}
		
	private:
		memory_resource* _upstream;
		struct LocalBuffer {
			char* begin = nullptr;
			char* current = nullptr;
			char const* end = nullptr;
		} _localBuffer;
		BufferNodeHeader* _bufferList;
		std::size_t _nextBufferSize;
	};
	
	inline char* monotonic_buffer_resource::BufferNodeHeader::_begin() const {
		return (char*)this + sizeof(*this);
	}
	
}


// MARK: - monitor_resource
namespace utl::pmr {
	
	class monitor_resource: public memory_resource {
	public:
		explicit monitor_resource(memory_resource* upstream): _upstream(upstream) {}
		
		memory_resource* upstream() { return _upstream; }
		memory_resource const* upstream() const { return _upstream; }
		
	private:
		void* do_allocate(std::size_t size, std::size_t alignment) override;
		
		void do_deallocate(void* ptr, std::size_t size, std::size_t alignment) override;
		
		bool do_is_equal(memory_resource const& rhs) const noexcept override { return upstream()->is_equal(rhs); }
		
		
	private:
		memory_resource* _upstream;
	};
	
}
