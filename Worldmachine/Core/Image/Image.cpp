#include "Image.hpp"

#include <utl/hash.hpp>

namespace worldmachine {
	
	std::size_t imageHash(std::span<float const> data, std::size_t summands) {
		if (summands == 0) {
			summands = 1;
		}
		std::size_t const stride = std::max<long>((long)std::bit_floor(data.size()) / (long)summands - 1, 1);
		std::size_t result = 0;
		for (auto i = data.begin(); i < data.end(); i += stride) {
			auto const x = utl::bit_cast<std::uint32_t>(*i);
			result = utl::hash_combine(result, x);
		}
		return result;
	}
	
	Image::Image(DataType dataType, mtl::uint2 size):
		m_dataType(dataType),
		m_size(size),
		m_data(_flatImageSize())
	{
		
	}
	
}
