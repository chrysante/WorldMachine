#pragma once

#include "Framework/ShaderMath.hpp"

namespace worldmachine {
	
	struct HeightmapVertex {
		metal::uint16_t x, y;
	};
	
	struct HeightmapRenderUniforms {
		metal::float4x4 modelMatrix;
		metal::float4x4 viewProjectionMatrix;
		metal::float2 quadResolution;
		float heightMultiplier;
		float baseLighting;
		float heightCapMin;
		float heightCapMax;
	};
	
}
