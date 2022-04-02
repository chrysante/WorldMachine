#include <Catch2/Catch2.hpp>

#include "Core/DataType.hpp"

using namespace worldmachine;

TEST_CASE("dataTypeSize") {
	CHECK(dataTypeSize(DataType::none)   == 0);
	CHECK(dataTypeSize(DataType::float1) == 1 * sizeof(float));
	CHECK(dataTypeSize(DataType::float2) == 2 * sizeof(float));
	CHECK(dataTypeSize(DataType::float3) == 3 * sizeof(float));
	CHECK(dataTypeSize(DataType::float4) == 4 * sizeof(float));
	CHECK(dataTypeSize(DataType::integer) == sizeof(int));
}
