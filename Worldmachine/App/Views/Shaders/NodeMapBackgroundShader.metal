#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"

#include "../SharedNetworkViewTypes.hpp"


using namespace metal;
using namespace worldmachine;

vertex VertexOut nodeMapBackgroundVertexShader(device Vertex2D const *vertexBuffer
																	  [[ buffer(0) ]],
											   uint vertexID          [[ vertex_id ]])
{
	return {
		.clipSpacePosition  = float4(vertexBuffer[vertexID].position * 2 - 1, 0, 1),
		.textureCoordinates = vertexBuffer[vertexID].uv
	};
}


fragment float4 nodeMapBackgroundFragmentShader(VertexOut in   [[ stage_in ]],
												device NetworkUniforms const& uniforms
															   [[ buffer(0) ]])
{
	float2 coords = in.textureCoordinates * uniforms.portal.size;
	
	float2 const position = uniforms.portal.position;
	
	coords += position;
	
	coords /= 100;
	
	float2 const fracCoords = fract(coords);
	float2 const linesXY = step(0.01 / uniforms.portal.scale, fracCoords);
	float const lines = 1 - min(linesXY.x, linesXY.y);
	
	return mix(uniforms.backgroundColor, uniforms.lineColor, lines);
}


