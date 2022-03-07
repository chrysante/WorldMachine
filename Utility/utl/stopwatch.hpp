#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)
#include <chrono>
#include <atomic>

namespace utl {
	
	template <typename Clock = std::chrono::high_resolution_clock>
	class stopwatch {
	public:		
		void pause() {
			if (_paused) {
				return;
			}
			auto const now = _now();
			_prev_duration += now - _start_point;
			_paused = true;
		}
		
		void resume() {
			if (!_paused) {
				return;
			}
			auto const now = _now();
			_start_point = now;
			_paused = false;
		}
		
		void reset() {
			if (_paused) {
				return;
			}
			auto const now = _now();
			_prev_duration = {};
			_start_point = now;
		}

		template <typename Rep = typename Clock::duration::rep, typename Units = typename Clock::duration>
		Rep elapsed_time() const
		{
			auto const now = _now();
			auto const dur = _paused ? _prev_duration : _prev_duration + (now - _start_point);
			auto const time_count = std::chrono::duration_cast<Units>(dur).count();
			
			return static_cast<Rep>(time_count);
		}
		
	private:
		static typename Clock::time_point _now() {
			std::atomic_thread_fence(std::memory_order_relaxed);
			auto const result = Clock::now();
			std::atomic_thread_fence(std::memory_order_relaxed);
			return result;
		}
		
	private:
		
		typename Clock::duration _prev_duration{};
		bool _paused = false;
		typename Clock::time_point _start_point = Clock::now();
	};

	using precise_stopwatch   = stopwatch<>;
	using system_stopwatch    = stopwatch<std::chrono::system_clock>;
	using monotonic_stopwatch = stopwatch<std::chrono::steady_clock>;
	
}
