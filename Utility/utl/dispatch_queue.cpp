#include "dispatch_queue.hpp"

#if 0
#define UTL_THREAD_DEBUG_LOG(...) utl_log(__VA_ARGS__)
#else
#define UTL_THREAD_DEBUG_LOG(...)
#endif

namespace utl {
	
	// MARK: - dispatch_item
	
	// MARK: - dispatch_group


	// MARK: - thread_pool
	thread_pool::thread_pool(): thread_pool(std::thread::hardware_concurrency()) {
		
	}
	
	thread_pool::thread_pool(std::size_t thread_count) {
		UTL_THREAD_DEBUG_LOG("Creating utl::thread_pool with {} threads.", thread_count);
		m_threads.reserve(thread_count);
		for (std::size_t i = 0; i < thread_count; ++i) {
			m_threads.emplace_back([this, i]{
				this->work_loop(i);
			});
		}
	}
	
	thread_pool::~thread_pool() {
		wait_for_current_tasks();
		m_mutex.lock();
		m_run = false;
		m_loop_cv.notify_all();
		m_mutex.unlock();
		for (auto& t: m_threads) {
			t.join();
		}
		UTL_THREAD_DEBUG_LOG("Destroyed utl::thread_pool with {} threads.", m_threads.size());
	}
	
	
	void thread_pool::submit(dispatch_item item) {
		submit(0, std::move(item));
	}
	
	void thread_pool::submit(std::size_t ID, dispatch_item item) {
		item.id = ID;
		std::unique_lock lock(m_mutex);
		++m_open_items;
		m_items.push_back(std::move(item));
		m_loop_cv.notify_one();
		UTL_THREAD_DEBUG_LOG("submitted one task, m_open_items = {}", m_open_items);
	}
	
	void thread_pool::cancel_current_tasks() {
		std::unique_lock lock(m_mutex);
		utl::small_vector<utl::function<void()>, 8> failure_handlers_copy;
		for (auto& i: m_items) {
			i._set_promise(false);
			auto const itr = m_failureHandlers.find(i.id);
			if (itr != m_failureHandlers.end()) {
				failure_handlers_copy.push_back(std::move(itr->second));
				m_failureHandlers.erase(itr);
			}
		}
		m_open_items -= m_items.size();
		UTL_THREAD_DEBUG_LOG("cancelling {} open tasks, m_open_items = {}", m_items.size(), m_open_items);
		m_items.clear();
		
		wait_for_current_tasks(std::move(lock));
		
		for (auto& f: failure_handlers_copy) {
			f();
		}
	}
	
	void thread_pool::work_loop(std::size_t index) {
		std::unique_lock lock(m_mutex);
		while (true) {
			if (!m_run)
				return;
			m_loop_cv.wait(lock, [&]{	// wait for work or destruction
				return !m_items.empty() || !m_run;
			});
			
			if (!m_items.empty()) {
				auto item = std::move(m_items.front());
				m_items.pop_front();
				lock.unlock();
				item.execute();
				
				lock.lock();
				UTL_THREAD_DEBUG_LOG("finished one task, m_open_items = {}");
				--m_open_items;
			}
			m_main_cv.notify_one();
		}
	}
	
	void thread_pool::wait_for_current_tasks() {
		wait_for_current_tasks(std::unique_lock(m_mutex));
	}
	
	void thread_pool::wait_for_current_tasks(std::unique_lock<std::mutex> lock) {
		m_main_cv.wait(lock, [this]{
			return m_open_items == 0;
		});
	}
	
	void thread_pool::add_failure_handler(std::size_t ID, utl::function<void()> f) {
		std::lock_guard lock(m_mutex);
		[[maybe_unused]] bool const success = m_failureHandlers.insert({ ID, std::move(f) }).second;
		utl_expect(success, "Please supply a unique ID");
	}
	
	void thread_pool::set_num_threads(std::size_t n) {
		this->~thread_pool();
		new ((void*)this) thread_pool(n);
	}
	
	// MARK: - dispatch_queue
	void dispatch_queue::defer(dispatch_item item) {
		std::lock_guard lock(m_mutex);
		m_deferred_items.push_back(std::move(item));
	}

	void dispatch_queue::execute_deferred() {
		std::unique_lock lock(m_mutex);
		if (!m_deferred_items.empty()) {
			// get all the items
			m_deferred_items_copy.clear();
			m_deferred_items_copy.reserve(m_deferred_items.size());
			std::move(m_deferred_items.begin(),
					  m_deferred_items.end(),
					  std::back_inserter(m_deferred_items_copy));
			m_deferred_items.clear();
		
			lock.unlock();
			
			// execute
			for (auto& item: m_deferred_items_copy) {
				item.execute();
			}
		}
	}
	
	
	// MARK: - concurrent_dispatch_queue
	void concurrent_dispatch_queue::async(dispatch_group&& group) {
		if (group.m_tasks.empty()) {
			return;
		}
		bool has_failure_handler = false;
		if (group.m_on_failure) {
			has_failure_handler = true;
			this->thread_pool::add_failure_handler(m_dispatch_group_id, std::move(group.m_on_failure));
		}
		if (group.m_promise.has_value() || group.m_on_completion) {
			struct Context {
				explicit Context(std::size_t count, std::optional<std::promise<bool>>&& p):
					count(count),
					promise(std::move(p))
				{}
				std::atomic<std::size_t> count;
				std::optional<std::promise<bool>> promise;
			};
			
			auto context = std::make_shared<Context>(group.m_tasks.size(), std::move(group.m_promise));
			for (auto& task: group.m_tasks) {
				dispatch_item item = [this, has_failure_handler,
									  id = m_dispatch_group_id,
									  on_completion = group.m_on_completion,
									  task = std::move(task),
									  context]
				{
					task();
					
					if (context->count.fetch_sub(1) == 1) {
						// we are done
						if (on_completion) {
							on_completion();
						}
						if (context->promise) {
							context->promise->set_value(true);
						}
						if (has_failure_handler) {
							std::lock_guard lock(this->thread_pool::m_mutex);
							// erase failure handler since we didnt fail
							[[maybe_unused]] bool const success = this->thread_pool::m_failureHandlers.erase(id);
							utl_assert(success);
						}
					}
				};
				this->thread_pool::submit(m_dispatch_group_id, std::move(item));
			}
		}
		else {
			// we dont need to worry about the handles
			for (auto& task: group.m_tasks) {
				this->thread_pool::submit(m_dispatch_group_id, std::move(task));
			}
		}
		++m_dispatch_group_id;
	}
	
	concurrent_dispatch_queue& concurrent_dispatch_queue::global() {
		static concurrent_dispatch_queue obj;
		return obj;
	}
	
	
	
}
