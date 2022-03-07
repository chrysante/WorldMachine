//
//  TestFunctions.metal
//  WorldMachine
//
//  Created by chrysante on 05.04.21.
//

#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"

#include "../SharedNetworkViewTypes.hpp"


using namespace metal;

struct EdgeVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
	float2 quadSize;
};

using namespace worldmachine;

vertex EdgeVSOutput edgeVertexShader(device Vertex2D const *vertexBuffer
																	  [[ buffer(0) ]],
									 device NetworkUniforms const& uniforms
																	  [[ buffer(1) ]],
									 device EdgeProxy const* edgeBuffer
																	  [[ buffer(2) ]],
									 uint vertexID                    [[ vertex_id ]],
									 uint instanceID                  [[ instance_id ]])
{
	auto const vert = vertexBuffer[vertexID];
	auto const edge = edgeBuffer[instanceID];
	
	float2 const edgeDiff = (edge.end.x != edge.begin.x && edge.end.y != edge.begin.y) ? edge.end - edge.begin : float2(1, 0);

	float2 const edgeVector = normalize(edgeDiff);
	float2x2 const rotation = float2x2(edgeVector, rotate90DegreesCounterClockwise(edgeVector));
	
	float2 const quadSize = float2(distance(edge.begin, edge.end), 0) + uniforms.edge.width;
	float2 const offsetVertexPosition =  rotation * (vert.position * quadSize - uniforms.edge.width / 2) + edge.begin;
	
	float4 finalPosition = float4(offsetVertexPosition, 0, 1);
#if 0
	finalPosition.z = mix(edge.beginIndex, edge.endIndex, vert.uv.x);
	finalPosition.z = uniforms.nodeCount - 1 - finalPosition.z;
#else
	finalPosition.z = uniforms.nodeCount - 1;
#endif
	
	return {
		.vertexPosition = uniforms.viewProjectionMatrix * finalPosition,
		.uv             = vert.uv,
		.quadSize       = quadSize
	};
}


fragment float4 edgeFragmentShader(EdgeVSOutput in    [[ stage_in ]],
								   device NetworkUniforms const& uniforms
													  [[ buffer(0) ]])
{
	auto const DF = roundedRectangleDistanceField(in.uv, in.quadSize, uniforms.edge.width / 2);

	if (DF > 1.0f) {
		discard_fragment();
	}

	return float4(uniforms.edgeColor.xyz, 1);
}


