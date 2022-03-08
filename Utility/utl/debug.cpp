#include "debug.hpp"

#include "stdio.hpp"
#include "bit.hpp"
#include "typeinfo.hpp"
#include "functional.hpp"

#include <thread>
#include <iomanip>
#include <ctime>

namespace utl::_private::debug {
	
	void _throw_runtime_error() {
		throw std::runtime_error("Assertion Failure");
	}
	
	std::false_type report_assertion_failure(assertion_kind kind,
											 log_data data,
											 char const* expr,
											 std::string expanded,
											 char const* message)
	{
		bool const has_msg = message && std::strlen(message) > 0;
		char const* kind_str = [&]{
			switch (kind) {
				case assertion_kind::assertion:
					return "Assertion";
				case assertion_kind::precondition:
					return "Precondition";
				case assertion_kind::postcondition:
					return "Postcondition";
			}
		}();
		
		auto const message_ends_with_dot = [&] {
			if (!has_msg)
				return false;
			char const last = message[strlen(message) - 1];
			return last == '.' || last == '!' || last == '?';
		}();
		
		utl::print("{}{}:{} {}{} failed{}: {}{}{} \n"
				   "\twith expansion {}  {}  {}\n"
				   "\t[{}{}{} : line {}]\n"
				   "\t[{}{}{}]\n"
				   "was not satisfied{}{}{}{}{}\n\n",
				   format_codes::gray, data.module_name, format_codes::reset,
				   
				   format_codes::red | format_codes::bold,
				   kind_str,
				   format_codes::reset | format_codes::gray,
				   format_codes::reset,
				   expr,
				   format_codes::gray,
				   
				   format_codes::reset | format_codes::background_red | format_codes::light_gray,
				   expanded,
				   format_codes::reset | format_codes::gray,
				   
				   format_codes::italic, data.function_name,
				   format_codes::reset | format_codes::gray,
				   data.line,
				   
				   format_codes::italic,
				   data.file_name,
				   format_codes::reset | format_codes::gray,
				   
				   has_msg ? ": " : ".",
				   has_msg ? format_codes::blue | format_codes::italic : format_codes::reset,
				   has_msg ? message : "",
				   !has_msg || message_ends_with_dot ? "" : ".",
				   format_codes::reset);
		
		return {};
	};
	
	void log_message(char const* msg) {
		utl::print(msg);
	}
	
	[[maybe_unused]] static char const* toStr(log_level l) {
		return std::array{
			" Trace ",
			" Info  ",
			" Debug ",
			"Warning",
			" Error ",
			" Fatal "
		}[utl::to_underlying(l)];
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
				utl_debugbreak();
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
	
	static std::uint64_t thread_id() {
		return utl::bit_cast<std::uint64_t>(std::this_thread::get_id()) % 997; /* greatest 3-digit prime */
	}
	
	static auto const main_thread_id = thread_id();
	
	static std::string time_in_HH_MM_SS_MMM() {
		using namespace std::chrono;

		// get current time
		auto now = system_clock::now();

		// get number of milliseconds for the current second
		// (remainder after division into seconds)
		auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

		// convert to std::time_t in order to convert to std::tm (broken time)
		auto timer = system_clock::to_time_t(now);

		// convert to broken time
		std::tm bt = *std::localtime(&timer);

		std::ostringstream oss;

		oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
		oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

		return oss.str();
	}
	
	void _do_log(log_data data, log_level level, std::string message) {
		static std::atomic<std::size_t> modNameLength = 0;
		update_maximum(modNameLength, strlen(data.module_name));
		
		int const function_name_width = 32;
		
		utl::print("{}{}[{:^{}}]{} [{}] [T:{}] [{}{:>{}}{}:{:4}]{} | {}{}{}\n",
				   format_codes::underline | format_codes::gray,
				   format_codes::bold, data.module_name, modNameLength,
				   format_codes::reset | format_codes::underline | format_codes::gray,
				   time_in_HH_MM_SS_MMM(),
				   thread_id() == main_thread_id ? "main" : utl::format("{:4}", thread_id()),
				   format_codes::italic,
				   truncate(qualified_function_name(data.function_name), function_name_width),
				   function_name_width,
				   format_codes::reset | format_codes::underline | format_codes::gray,
				   data.line,
				   
				   format_codes::reset,
				   logLevelMessageFormat(level),
				   message,
				   format_codes::reset);
		auto [stream, lock] = get_global_output_stream();
		std::flush(*stream._stream);
	}
}

