#include "log.hpp"

#include "stdio.hpp"
#include "typeinfo.hpp"

#include <iomanip>


namespace utl {
	
	void logger::do_log(log_message msg) {
		for (auto& l: _listeners) {
			l(msg);
		}
	}
	
	[[maybe_unused]] static format_codes::format_code_array logLevelFormat(log_level l) {
		switch (l) {
			case log_level::trace:
				return format_codes::gray | format_codes::bold;
			case log_level::info:
				return format_codes::green | format_codes::bold;
			case log_level::debug:
				return format_codes::blue | format_codes::bold;
			case log_level::warning:
				return format_codes::dark_yellow | format_codes::bold;
			case log_level::error:
				return format_codes::red | format_codes::bold;
			case log_level::fatal:
				return format_codes::background_red | format_codes::light_gray | format_codes::bold;
			default:
				utl_debugbreak("invalid log_level");
		}
	}
	
	[[maybe_unused]] static format_codes::format_code_array logLevelMessageFormat(log_level l) {
		return [&]() -> format_codes::format_code_array {
			switch (l) {
				case log_level::trace:
					return format_codes::gray;
				case log_level::info:
					return format_codes::green;
				case log_level::debug:
					return format_codes::blue;
				case log_level::warning:
					return format_codes::dark_yellow;
				case log_level::error:
					return format_codes::red;
				case log_level::fatal:
					return format_codes::background_red | format_codes::bold | format_codes::light_gray;
				default:
					utl_debugbreak("invalid log_level");
			}
		}() | format_codes::italic;
	}
	
	static std::string truncate(std::string str, int width) {
		if (str.size() > width) {
			auto result = str.substr(str.size() - width, width);
			for (int i = 0; i < std::min(3, width); ++i) {
				result[i] = '.';
			}
			return result;
		}
		return str;
	}
	
	template<typename T>
	static void update_maximum(std::atomic<T>& maximum_value, T const& value) noexcept {
		T prev_value = maximum_value;
		while(prev_value < value &&
				!maximum_value.compare_exchange_weak(prev_value, value))
			{}
	}

	std::string time_in_HH_MM_SS_MMM(std::chrono::time_point<std::chrono::system_clock> tp) {
		using namespace std::chrono;

		// get number of milliseconds for the current second
		// (remainder after division into seconds)
		auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;

		// convert to std::time_t in order to convert to std::tm (broken time)
		auto timer = system_clock::to_time_t(tp);

		// convert to broken time
		std::tm bt = *std::localtime(&timer);

		std::ostringstream oss;

		oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
		oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

		return oss.str();
	}
	
	
	static void _stdout_listener(log_message const& message) {
		static std::atomic<std::size_t> modNameLength = 0;
		update_maximum(modNameLength, message.logger_name.size());
		
		int const function_name_width = 32;
		
		using namespace std::chrono;
		
		utl::print("{}{}[{:^{}}]{} [{}] [T:{}] [{}{:>{}}{}:{:4}]{} | {}{}{}\n",
				   format_codes::underline | format_codes::gray,
				   format_codes::bold, message.logger_name, modNameLength,
				   format_codes::reset | format_codes::underline | format_codes::gray,
				   time_in_HH_MM_SS_MMM(message.time),
				   utl::format("{:6}", utl::bit_cast<std::size_t>(message.thread_id)),
				   format_codes::italic,
				   truncate(qualified_function_name(message.source_info.function), function_name_width),
				   function_name_width,
				   format_codes::reset | format_codes::underline | format_codes::gray,
				   message.source_info.line,
				   
				   format_codes::reset,
				   logLevelMessageFormat(message.level),
				   message.message,
				   format_codes::reset);
	}
	
	utl::function<void(log_message)> make_stdout_listener() {
		return _stdout_listener;
	}
	
}
