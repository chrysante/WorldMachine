#include "DataType.hpp"

#include <ostream>

#include "Core/Debug.hpp"

namespace worldmachine {
	
	std::ostream& operator<<(std::ostream& str, DataType t) {
		auto const Int = utl::to_underlying(t);
		if (!Int) {
			return str << "none";
		}
		return str << std::array{
			"float1",
			"float2",
			"float3",
			"float4",
			"integer"
		}[utl::log2(Int)];
	}
	
	std::size_t dataTypeSize(DataType t) {
		if (t == DataType::none) {
			return 0;
		}
		if (auto const i = utl::to_underlying(t);
			i <= utl::to_underlying(DataType::float4))
		{
			return sizeof(float) * (1 + utl::log2(i));
		}
		if (t == DataType::integer) {
			return sizeof(int);
		}
		WM_DebugBreak("Unknown Data Type");
	}
	
}
