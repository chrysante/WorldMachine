#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"

#include "../SharedNetworkViewTypes.hpp"

using namespace metal;

struct NodeShadowVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
	float2 nodeSize;
	float2 quadSize;
};

vertex NodeShadowVSOutput nodeShadowVertexShader(device worldmachine::Vertex2D const *vertexBuffer
																	  [[ buffer(0) ]],
												 device worldmachine::NetworkUniforms const& uniforms
																	  [[ buffer(1) ]],
												 device float3 const* nodePositionBuffer
																	  [[ buffer(2) ]],
												 device float2 const* nodeSizeBuffer
																	  [[ buffer(3) ]],
												 uint vertexID        [[ vertex_id ]],
												 uint instanceID      [[ instance_id ]])
{
	auto const vert = vertexBuffer[vertexID];
	auto const nodePosition = nodePositionBuffer[instanceID];
	
	auto const nodeSize = nodeSizeBuffer[instanceID];
	auto const quadSize = nodeSize + 2 * uniforms.node.outerShadowSize;
	
	float4 const offsetVertexPosition = float4((vert.position - 0.5) * quadSize + nodePosition.xy + uniforms.shadowOffset,
											   nodePosition.z + 0.1, // +0.1 to render behind the nodes
											   1);
	
	return {
		.vertexPosition = uniforms.viewProjectionMatrix * offsetVertexPosition,
		.uv             = vert.uv,
		.nodeSize       = nodeSize,
		.quadSize       = quadSize
	};
}

static float shadow(float2 uv, float2 quadSize, float2 nodeSize,
					device worldmachine::NetworkUniforms const& uniforms)
{
	auto const cornerDistanceField = roundedRectangleDistanceField(uv, quadSize, uniforms.node.innerShadowSize + uniforms.node.outerShadowSize);
	
	
	return uniforms.node.shadowIntensity * pow(1 - clamp(cornerDistanceField, 0.0f, 1.0f), 2);
}


fragment float4 nodeShadowFragmentShader(NodeShadowVSOutput in [[ stage_in ]],
										 device worldmachine::NetworkUniforms const& uniforms
														       [[ buffer(0) ]])
{
	auto const shadow = ::shadow(in.uv, in.quadSize, in.nodeSize, uniforms);

	return float4(0, 0, 0, shadow);
}


