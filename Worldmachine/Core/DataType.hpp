#pragma once

#include "Core/Base.hpp"

#include <ostream>
#include <mtl/mtl.hpp>
#include <utl/utility.hpp>

namespace worldmachine {
	
	enum struct DataType: unsigned {
		none   = 0,
		
		float1 = 1 << 0, /* 1 */
		float2 = 1 << 1, /* 2 */
		float3 = 1 << 2, /* 4 */
		float4 = 1 << 3  /* 8 */
	};

	UTL_ENUM_OPERATORS(DataType);
	
	inline std::ostream& operator<<(std::ostream& str, DataType t) {
		auto const Int = utl::to_underlying(t);
		if (!Int) {
			return str << "none";
		}
		return str << std::array{
			"float",
			"float2",
			"float3",
			"float4"
		}[utl::log2(Int)];
	}

	inline std::size_t dataTypeSize(DataType t) {
		return sizeof(float) * utl::to_underlying(t);
	}


	template <DataType DT>
	struct DataTypeToType {
	private:
		static auto _result() {
				 if constexpr (DT == DataType::float1) { return      float {}; }
			else if constexpr (DT == DataType::float2) { return mtl::float2{}; }
			else if constexpr (DT == DataType::float3) { return mtl::float3{}; }
			else if constexpr (DT == DataType::float4) { return mtl::float4{}; }
		}
		
	public:
		using type = decltype(_result());
	};
	
}


