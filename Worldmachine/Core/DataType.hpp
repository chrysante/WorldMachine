#pragma once

#include "Core/Base.hpp"

#include <iosfwd>
#include <mtl/mtl.hpp>
#include <utl/common.hpp>
#include <utl/utility.hpp>


namespace worldmachine {
	
	enum struct DataType: unsigned {
		none   = 0,
		
		float1  = 1 << 0,
		float2  = 1 << 1,
		float3  = 1 << 2,
		float4  = 1 << 3,
		integer = 1 << 4
	};

	UTL_ENUM_OPERATORS(DataType);
	
	std::ostream& operator<<(std::ostream& str, DataType t);

	std::size_t dataTypeSize(DataType t);


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


