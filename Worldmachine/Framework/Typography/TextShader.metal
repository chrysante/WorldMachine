#if 0


#include <metal_stdlib>

#include "../Renderer/Vertex2D.hpp"
#include "../Renderer/ShaderCommon.hpp"
#include "TextRenderData.hpp"
#include "TypographyShader.h"
#include "../NetworkView/Shaders/SharedNetworkViewTypes.hpp"


using namespace metal;
using namespace worldmachine;

struct TextVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
};

vertex TextVSOutput textVertexShader(device float2 const* vertexBuffer      [[ buffer(0) ]],
									 device LetterData const* letterBuffer  [[ buffer(1) ]],
									 device GlyphData const* glyphBuffer    [[ buffer(2) ]],
									 device NetworkUniforms const& uniforms [[ buffer(3) ]],
							         uint vertexID                          [[ vertex_id ]]) {
	auto const result = textVertex(vertexBuffer[vertexID % 6],
								   letterBuffer,
								   glyphBuffer,
								   vertexID);
	
	auto const vertexPosition = result.position.xy * 300;
	
	
	
	return {
		.vertexPosition = uniforms.viewMatrix * float4(vertexPosition, 0, 1),
		.uv             = result.uv
	};
}



fragment float4 textFragmentShader(TextVSOutput     in          [[ stage_in ]],
								   texture2d<float> msdfTexture [[ texture(0) ]],
								   sampler          sampler2D   [[ sampler(0) ]])
{
	auto const sample = textSample(in.uv, msdfTexture, sampler2D);
	return float4(1, 1, 1, sample.x);
}

#endif
