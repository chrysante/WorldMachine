#include <iostream>

#include <Catch2/Catch2.hpp>

#include "Core/Image/Image.hpp"

#include <utl/utility.hpp>
#include <utl/test/terminates.hpp>

TEST_CASE("imageHashTerminates") {
	using namespace worldmachine;
	
	Image img{ DataType::float2, { 128, 128 } };
	
	using namespace std::chrono_literals;
	
	CHECK(utl::terminates(100ms, [&]{
		imageHash(img.toFloatSpan(), 100);
	}));
}

TEST_CASE("imageHash") {
	
	
	
}
