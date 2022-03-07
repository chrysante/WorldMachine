#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include "debug.hpp"
#include "type_traits.hpp"
#include "functional.hpp"
#include "vector.hpp"
#include "concepts.hpp"
#include "hashmap.hpp"

#include <thread>
#include <future>
#include <atomic>
#include <queue>
#include <optional>


namespace utl {
	
	namespace _private {
		using task_t = utl::unique_function<void()>;
	}
	
	// MARK: - dispatch_handle
	class dispatch_handle {
	public:
		bool wait() {
			return m_future.get();
		}
		
	private:
		friend class dispatch_item;
		friend class dispatch_group;
		explicit dispatch_handle(std::future<bool>&& f): m_future(std::move(f)) {}
	
	private:
		std::future<bool> m_future;
	};
	
	// MARK: - dispatch_item
	class dispatch_item {
		friend class thread_pool;
	public:
		dispatch_item(invocable auto&& function):
			_function(UTL_FORWARD(function)) {}
		
		void execute() {
			_function();
			_set_promise(true);
		}
		
		dispatch_handle get_handle() {
			utl_expect(!_promise.has_value(), "get_handle() must not be called more than once on any dispatch item");
			_promise = std::promise<bool>{};
			return dispatch_handle(_promise->get_future());
		}
		
	private:
		void _set_promise(bool value) {
			if (_promise) {
				_promise->set_value(value);
			}
		}
		
	private:
		_private::task_t _function;
		std::optional<std::promise<bool>> _promise;
		std::size_t id = 0;
	};
	
	// MARK: - dispatch_group
	class dispatch_group {
	public:
		template <typename F,
				  UTL_ENABLE_IF(std::is_invocable<F>::value),
				  UTL_ENABLE_IF(std::is_copy_constructible<F>::value)>
		void add(F&& function) {
			m_tasks.emplace_back(std::forward<F>(function));
		}
		
		template <typename Range,
				  UTL_ENABLE_IF(is_range<Range>::value),
				  typename F,
				  UTL_ENABLE_IF(std::is_invocable<F, typename range_traits<Range>::reference_type>::value)>
		void add_for_each(Range&& range, F const& function) {
			for (auto&& i: range) {
				if constexpr (std::is_copy_constructible<std::remove_reference_t<decltype(i)>>::value) {
					this->add([value = i, function]{
						function(value);
					});
				}
				else  {
					static_assert(std::is_move_constructible<std::remove_reference_t<decltype(i)>>::value,
								  "type in range needs to be either copy or move constructible");
					this->add([value = std::move(i), function]() mutable {
						function(std::move(value));
					});
				}
			}
		}
		
		template <typename Range,
				  UTL_ENABLE_IF(is_range<Range>::value),
				  typename F,
				  UTL_ENABLE_IF(std::is_invocable<F>::value)>
		void add_for_each(Range&& range, F const& function) {
			add_for_each(UTL_FORWARD(range), [function](auto&&){ function(); });
		}
		
		std::size_t size() const { return m_tasks.size(); }
		
		[[nodiscard]] dispatch_handle get_handle() {
			utl_expect(!m_promise.has_value(), "get_handle() must not be called more than once");
			m_promise = std::promise<bool>{};
			return dispatch_handle(m_promise->get_future());
		}
		
		void on_completion(utl::function<void()> f) {
			m_on_completion = std::move(f);
		}
		void on_failure(utl::function<void()> f) {
			m_on_failure = std::move(f);
		}
		
	private:
		friend class concurrent_dispatch_queue;
		
	private:
		utl::vector<_private::task_t> m_tasks;
		std::optional<std::promise<bool>> m_promise;
		utl::function<void()> m_on_completion, m_on_failure;
	};
	
	// MARK: - thread_pool
	class thread_pool {
		friend class concurrent_dispatch_queue;
	public:
		thread_pool();
		explicit thread_pool(std::size_t thread_count);
		~thread_pool();
		
		void submit(dispatch_item);
		
		void cancel_current_tasks();
		void wait_for_current_tasks();
		std::size_t num_threads() const { return m_threads.size(); }
		void set_num_threads(std::size_t);
		
	private:
		void work_loop(std::size_t index);
		void wait_for_current_tasks(std::unique_lock<std::mutex>);
		
		void submit(std::size_t ID, dispatch_item);
		void add_failure_handler(std::size_t ID, utl::function<void()>);
		
	private:
		utl::vector<std::thread> m_threads;
		bool m_run = true;
		std::size_t m_open_items = 0;
		std::mutex m_mutex;
		std::condition_variable m_loop_cv;
		std::condition_variable m_main_cv;
		std::deque<dispatch_item> m_items;
		utl::hashmap<std::size_t, utl::function<void()>> m_failureHandlers;
	};
	
	
	// MARK: - dispatch_queue
	class dispatch_queue {
	public:
		void defer(dispatch_item);
		
		void execute_deferred();
		
	private:
		utl::vector<dispatch_item> m_deferred_items;
		utl::vector<dispatch_item> m_deferred_items_copy;
		std::mutex m_mutex;
	};
	
	
	// MARK: - concurrent_dispatch_queue
	class concurrent_dispatch_queue: private thread_pool {
	public:
		concurrent_dispatch_queue() = default;
		explicit concurrent_dispatch_queue(std::size_t thread_count):
			thread_pool(thread_count)
		{}
		
		void async(dispatch_item&& item) {
			this->thread_pool::submit(std::move(item));
		}
		
		void async(dispatch_group&&);
		
		using thread_pool::cancel_current_tasks;
		void wait_for_current_tasks() {
			this->thread_pool::wait_for_current_tasks();
		}
		using thread_pool::num_threads;
		using thread_pool::set_num_threads;
				
		static concurrent_dispatch_queue& global();
		
	private:
		std::size_t m_dispatch_group_id = 0;
	};
	
	
}


