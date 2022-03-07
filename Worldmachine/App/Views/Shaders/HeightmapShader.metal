//
//  TestFunctions.metal
//  WorldMachine
//
//  Created by chrysante on 05.04.21.
//

#include <metal_stdlib>

#include "../../../Framework/Vertex2D.hpp"
#include "../../../Framework/ShaderCommon.hpp"
#include "HeightmapRenderUniforms.hpp"


using namespace metal;
using namespace worldmachine;


struct HeightmapVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
};

vertex HeightmapVSOutput heightmapVertexShader(device HeightmapVertex const*         vertexBuffer [[ buffer(0)  ]],
											   device HeightmapRenderUniforms const& uniforms     [[ buffer(1)  ]],
											   texture2d<float>                      heightmap    [[ texture(0) ]],
											   sampler                               sampler2D    [[ sampler(0) ]],
											   uint                                  vertexID     [[ vertex_id  ]])
{
	HeightmapVertex const vert = vertexBuffer[vertexID];
	
	float2 uv = float2(vert.x, vert.y) / uniforms.quadResolution;
	
	float2 positionXY = (float2(vert.x, vert.y) - uniforms.quadResolution / 2);
	positionXY *= 2 / uniforms.quadResolution.x;
	
	float height = heightmap.sample(sampler2D, uv).r;
	height = clamp(height, uniforms.heightCapMin, uniforms.heightCapMax);
	
	float4 finalPositionMS = float4(positionXY,
									(height - 0.5) * uniforms.heightMultiplier,
									1);
	
	return {
		.vertexPosition = uniforms.viewProjectionMatrix * uniforms.modelMatrix * finalPositionMS,
		.uv             = uv
	};
}

static float3 calculateMSNormals(float2 uv,
								 float heightMultiplier,
								 texture2d<float> heightmap,
								 sampler sampler2D,
								 float2 quadResolution,
								 float2 heightCap)
{
	float height = heightmap.sample(sampler2D, uv).r;
	height = clamp(height, heightCap.x, heightCap.y);
	float2 xOffset = float2(1, 0) / quadResolution;
	float2 yOffset = float2(0, 1) / quadResolution;
	float heightXplus1 = heightmap.sample(sampler2D, uv + xOffset).r;
	heightXplus1 = clamp(heightXplus1, heightCap.x, heightCap.y);
	float heightYplus1 = heightmap.sample(sampler2D, uv + yOffset).r;
	heightYplus1 = clamp(heightYplus1, heightCap.x, heightCap.y);
	
	float3 tangent   = float3(xOffset, 0.5 * heightMultiplier * (heightXplus1 - height));
	float3 cotangent = float3(yOffset, 0.5 * heightMultiplier * (heightYplus1 - height));
	return normalize(cross(tangent, cotangent));
}

float3 lighting(float3 normal, float3 lightDir, float ambient) {
	float const shade = pow(dot(normal, normalize(-lightDir)), 2);
	float3 const result = mix(shade, 1, ambient);
	return result;
}

fragment float4 heightmapFragmentShader(HeightmapVSOutput                     in        [[ stage_in   ]],
										device HeightmapRenderUniforms const& uniforms  [[ buffer(0)  ]],
										texture2d<float>                      heightmap [[ texture(0) ]],
										sampler                               sampler2D [[ sampler(0) ]])
{
	float3 msNormal = calculateMSNormals(in.uv, uniforms.heightMultiplier, heightmap, sampler2D,
										 uniforms.quadResolution,
										 {uniforms.heightCapMin, uniforms.heightCapMax});
	float3 wsNormal = (uniforms.modelMatrix * float4(msNormal, 0)).xyz;
	float3 lightDir = float3(1, 0, 1);
	
	float3 lit = lighting(wsNormal, lightDir, uniforms.baseLighting);
	
	float height = heightmap.sample(sampler2D, in.uv).r;
	
	float3 lowerColor = mix(float3(.3, .2, .1), float3(.2, .7, .1), clamp(height * 2, 0.f, 1.f));
	float3 color = mix(lowerColor, float3(.7, .7, .7), clamp(height * 2 - 1, 0.f, 1.f));
	
	return float4(color * lit, 1);
}

