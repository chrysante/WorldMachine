#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"
#include "Framework/Typography/TextRenderData.hpp"
#include "Framework/Typography/TypographyShader.hpp"
#include "../SharedNetworkViewTypes.hpp"


using namespace metal;
using namespace worldmachine;

vertex TextVertex nodeNameVertexShader(device float2 const* vertexBuffer         [[ buffer(0) ]],
									   device LetterData const* letterBuffer     [[ buffer(1) ]],
									   device GlyphData const* glyphBuffer       [[ buffer(2) ]],
									   device NetworkUniforms const& uniforms    [[ buffer(3) ]],
									   device LabelRenderData const& labelData   [[ buffer(4) ]],
									   device float3 const* nodePositionBuffer   [[ buffer(5) ]],
									   device float2 const* nodeSizeBuffer       [[ buffer(6) ]],
									   uint vertexID                             [[ vertex_id ]],
									   uint instanceID                           [[ instance_id ]]) {
	auto vertexIn = vertexBuffer[vertexID % 6];
	
	auto const textSize = labelData.size;
	
	
	auto result = makeTextVertex(vertexIn,
								 textSize,
								 letterBuffer + nodeNameMaxRenderSize * instanceID,
								 glyphBuffer,
								 vertexID);
	
	auto const nodePosition = nodePositionBuffer[instanceID];
	auto const nodeSize     = nodeSizeBuffer[instanceID];
	
	result.position.xy += nodePosition.xy;
	result.position.y -= nodeSize.y / 2 + textSize;
	result.position.z = nodePosition.z;
	result.position.w = 1;
	result.position = uniforms.viewProjectionMatrix * result.position;
	
	result.pixelRange = labelData.pixelRange;
	
	result.textureIndex = 0;
	
	return result;
}



fragment float4 nodeNameFragmentShader(TextVertex                    in          [[ stage_in ]],
									   device FontData const&        fontData    [[ buffer(0) ]],
									   device NetworkUniforms const& uniforms    [[ buffer(1) ]],
									   texture2d<float>              msdfTexture [[ texture(0) ]],
									   sampler                       sampler2D   [[ sampler(0) ]])
{
	auto const sample = textSample(in.uv, in.pixelRange, msdfTexture, sampler2D);
	return float4(uniforms.labelColor.rgb, sample.x);
}

