//
//  TestFunctions.metal
//  WorldMachine
//
//  Created by chrysante on 05.04.21.
//

#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"


using namespace metal;
using namespace worldmachine;

struct Image2DVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
};

vertex Image2DVSOutput image2DVertexShader(device Vertex2D const *vertexBuffer [[ buffer(0) ]],
										   uint vertexID                       [[ vertex_id ]]) {
	auto const vert = vertexBuffer[vertexID];
	
	return {
		.vertexPosition = float4(vert.position * 2 - 1, 0, 1),
		.uv             = vert.uv
	};
}


fragment float4 image2DFragmentShader(Image2DVSOutput   in        [[ stage_in ]],
									  texture2d<float>  tex2D     [[ texture(0) ]],
									  sampler           sampler2D [[ sampler(0) ]],
									  device int const& channels  [[ buffer(0) ]])
{
	if (channels == 1) {
		return float4(tex2D.sample(sampler2D, in.uv).rrr, 1);
	}
	else if (channels == 2) {
		return float4(tex2D.sample(sampler2D, in.uv).rg, 0, 1);
	}
	else {
		return float4(tex2D.sample(sampler2D, in.uv).rrr, 1);
	}
}

