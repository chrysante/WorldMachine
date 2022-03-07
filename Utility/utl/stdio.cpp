#include "stdio.hpp"

#include <iostream>
#include <mutex>

namespace utl::_private {
	/// We don't deallocate these so we can always use utl::print(), even in destructors of statics
	static std::mutex& _print_mutex() {
		static std::mutex* x = new std::mutex();
		return *x;
	}
	static stream_ref_wrapper& _global_stream() {
		static stream_ref_wrapper* x = new stream_ref_wrapper(std::cout);
		return *x;
	}
	
	
	
}

namespace utl {
	
	stream_ref_wrapper::stream_ref_wrapper(std::ostream& stream,
										   bool force_colors):
		_stream(&stream),
		_using_colors(force_colors ||
					  ((_stream == &std::cout ||
						_stream == &std::cerr) &&
					   std::getenv("TERM") != nullptr))
	{
		
	}
	
	std::tuple<stream_ref_wrapper, std::unique_lock<std::mutex>>
	get_global_output_stream() {
		using namespace _private;
		std::unique_lock<std::mutex> lock(_print_mutex());
		return { _global_stream(), std::move(lock) };
	}
	
	stream_ref_wrapper set_global_output_stream(stream_ref_wrapper stream)
	{
		using namespace _private;
		auto [old, lock] = get_global_output_stream();
		_global_stream() = stream;
		return old;
	}
}

