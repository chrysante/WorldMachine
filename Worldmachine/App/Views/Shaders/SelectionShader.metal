#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"

#include "../SharedNetworkViewTypes.hpp"


using namespace metal;

struct SelectionVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
	float2 quadSize;
};

vertex SelectionVSOutput selectionVertexShader(device worldmachine::Vertex2D const *vertexBuffer
																	 [[ buffer(0) ]],
											   device worldmachine::NetworkUniforms const& uniforms
																	 [[ buffer(1) ]],
											   device float2 const* selectionData
																	 [[ buffer(2) ]],
											   uint vertexID                   [[ vertex_id ]])
{
	auto const vert = vertexBuffer[vertexID];
	
	auto const quadPosition = selectionData[0];
	auto const quadSize = selectionData[1];
	
	auto const offsetVertexPosition = vert.position * quadSize + quadPosition;
	
	return {
		.vertexPosition = uniforms.viewProjectionMatrix * float4(offsetVertexPosition, 0, 1),
		.uv             = vert.uv,
		.quadSize       = abs(quadSize)
	};
}


fragment float4 selectionFragmentShader(SelectionVSOutput in    [[ stage_in ]],
										device worldmachine::NetworkUniforms const& uniforms
													            [[ buffer(0) ]])
{
	auto const borderSize = uniforms.selection.borderSize / uniforms.portal.scale;
	auto const borderUV = (in.uv * (in.quadSize + 2 * borderSize) - borderSize) / in.quadSize;

	auto const border = isInZeroOneRange(borderUV);
	
	auto const color = mix(uniforms.selection.borderColor, uniforms.selection.color, border);
	
	return color;
}


