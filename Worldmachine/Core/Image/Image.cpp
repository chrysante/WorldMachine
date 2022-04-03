#include "Image.hpp"

#include <utl/hash.hpp>
#include <utl/iterator.hpp>

namespace worldmachine {
	
	std::size_t imageHash(std::span<float const> data, std::size_t summands) {
		if (summands == 0) {
			summands = 1;
		}
		std::size_t const stride = std::max<long>((long)std::bit_floor(data.size()) / (long)summands - 1, 1);
		
		auto convert = [](float x) { return utl::bit_cast<std::uint32_t>(x); };
		
		return utl::hash_combine_range(utl::conversion_iterator(utl::stride_iterator(data.begin(), stride), convert),
									   data.begin() + utl::round_down(data.size(), stride));
	}
	
	Image::Image(DataType dataType, mtl::uint2 size):
		m_dataType(dataType),
		m_size(size),
		m_data(_flatImageSize())
	{
		
	}
	
}
