#include <metal_stdlib>

#include "Framework/Vertex2D.hpp"
#include "Framework/ShaderCommon.hpp"

#include "../SharedNetworkViewTypes.hpp"


using namespace metal;

struct NodeVSOutput {
	float4 vertexPosition [[ position ]];
	float2 uv;
	float2 nodeSize;
	float2 quadSize;
	float buildProgress;
	
	bool isSelected;
	bool isBuilt;
	int  category;
	worldmachine::PinCount<float> pinCount;
};

vertex NodeVSOutput nodeVertexShader(device worldmachine::Vertex2D const *vertexBuffer
																	  [[ buffer(0) ]],
									 device worldmachine::NetworkUniforms const& uniforms
																	  [[ buffer(1) ]],
									 device float3 const* nodePositionBuffer
																	  [[ buffer(2) ]],
									 device float2 const* nodeSizeBuffer
																	  [[ buffer(3) ]],
									 device int const* nodeCategoryBuffer
																	  [[ buffer(4) ]],
									 device worldmachine::NodeFlags const* nodeFlagBuffer
																	  [[ buffer(5) ]],
									 
									 device worldmachine::PinCount<> const* pinCountBuffer
																	  [[ buffer(6) ]],
									 
									 device float const* buildProgress
																	  [[ buffer(7) ]],
									 
									 uint vertexID                    [[ vertex_id ]],
									 uint instanceID                  [[ instance_id ]])
{
	auto const vert = vertexBuffer[vertexID];
	float3 const nodePosition = nodePositionBuffer[instanceID];
	
	auto const nodeSize = nodeSizeBuffer[instanceID];
	auto const quadSize = nodeSize;
	
	float4 const offsetVertexPosition = float4((vert.position - 0.5) * quadSize + nodePosition.xy,
											   nodePosition.z, 1);
	
	return {
		.vertexPosition = uniforms.viewProjectionMatrix * offsetVertexPosition,
		.uv             = vert.uv,
		.nodeSize       = nodeSize,
		.quadSize       = quadSize,
		
		.buildProgress	= buildProgress[instanceID],
		
		.isSelected     = static_cast<bool>(nodeFlagBuffer[instanceID] & worldmachine::NodeFlags::selected),
		.isBuilt        = static_cast<bool>(nodeFlagBuffer[instanceID] & worldmachine::NodeFlags::built),
		.category       = nodeCategoryBuffer[instanceID],
		.pinCount = {
			.input          = static_cast<float>(pinCountBuffer[instanceID].input),
			.output         = static_cast<float>(pinCountBuffer[instanceID].output),
			.parameterInput = static_cast<float>(pinCountBuffer[instanceID].parameterInput),
			.maskInput      = static_cast<float>(pinCountBuffer[instanceID].maskInput)
		}
	};
}

static auto circleArrayDistanceField(float2 uv, float2 rectSize, float circleRadius, float spacingRadius, float2 offset, float2 count) {
	uv *= rectSize;
	auto const distanceFieldUV = (uv - spacingRadius - offset) / (2.0f * spacingRadius);
	auto const DF = (spacingRadius / circleRadius) * 2.0f * length(fract(distanceFieldUV) - 0.5f);
	
	
	auto const cutShapeUV = (uv + float2(0, spacingRadius) - offset) / (count * float2(1.0f, 2.0f) * spacingRadius);
	auto const cutShape = isInZeroOneRange(cutShapeUV);
	
	return cutShape ? DF : 999.999f;
}

static float pinArrayShape(float2 uv,
								 float2 nodeSize,
								 float pinRadius,
								 float pinSpacingRadius,
								 float offset,
								 float count,
								 Stepper stepper) {
	auto const DF = circleArrayDistanceField(uv,
											 nodeSize,
											 pinRadius,
											 pinSpacingRadius,
											 float2(0.0f, offset + pinSpacingRadius),
											 float2(1.0f, count));
	return stepper(1.0f, DF);
}

namespace {
	struct NodeShapeResult {
		float nodeShape;
		float selectionBand;
		float unbuiltBand;
	};
}

static float _nodeShapeImpl(float2 uv,
							float2 nodeSize,
							float pinRadius,
							float pinSpacingRadius,
							float parameterPinRadius,
							float parameterPinSpacingRadius,
							float offset,
							worldmachine::PinCount<float> count,
							Stepper stepper) {
	auto const inputUV     = float2(    uv.x, 1 - uv.y);
	auto const outputUV    = float2(1 - uv.x, 1 - uv.y);
	auto const parameterUV = float2(1 - uv.y,     uv.x);
	auto const maskUV      = float2(    uv.y,     uv.x);
	
	// input array
	auto const inputArrayShape = pinArrayShape(inputUV, nodeSize,
													 pinRadius,
													 pinSpacingRadius,
													 offset,
													 count.input,
													 stepper);
	// output array
	auto const outputArrayShape = pinArrayShape(outputUV, nodeSize,
													  pinRadius,
													  pinSpacingRadius,
													  offset,
													  count.output,
													  stepper);
	// parameter array
	auto const parameterArrayShape = pinArrayShape(parameterUV, float2(nodeSize.y, nodeSize.x),
														 parameterPinRadius,
														 parameterPinSpacingRadius,
														 offset,
														 count.parameterInput,
														 stepper);
	// mask array
	auto const maskArrayShape = pinArrayShape(maskUV, float2(nodeSize.y, nodeSize.x),
													pinRadius,
													pinSpacingRadius,
													offset,
													count.maskInput,
													stepper);
	
	return inputArrayShape * outputArrayShape * parameterArrayShape * maskArrayShape;
}

static NodeShapeResult nodeShape(float2 uv, float2 nodeSize, worldmachine::PinCount<float> pinCount,
								 device worldmachine::NetworkUniforms const& uniforms, float buildProgress, bool isSelected, bool isBuilt)
{
	auto const cornerDistanceField = roundedRectangleDistanceField(uv, nodeSize, uniforms.node.cornerRadius);
	
	auto const stepper = Stepper(uniforms.portal.scale);
	
	auto const selectionBandWidth = uniforms.node.selectionBandWidth / sqrt(max(1.0f, uniforms.portal.scale));
	auto const unbuiltBandWidth   = uniforms.node.unbuiltBandWidth * (1 - buildProgress) / sqrt(max(1.0f, uniforms.portal.scale));
	
	auto const nodeShape = _nodeShapeImpl(uv, nodeSize,
										  uniforms.node.pinSize / 2,
										  uniforms.node.pinSpacing / 2,
										  uniforms.node.parameterPinSize / 2,
										  uniforms.node.parameterPinSpacing / 2,
										  uniforms.node.cornerRadius,
										  pinCount, stepper) * (1.0f - stepper(1.0f, cornerDistanceField));
	
	float selectionBandShape = 0.0f;
	if (isSelected) {
		auto const selectionBandShapeCorners = 1.0f - stepper(1.0f - selectionBandWidth / uniforms.node.cornerRadius,
															  cornerDistanceField);
		selectionBandShape = 1.0f - _nodeShapeImpl(uv, nodeSize,
												   uniforms.node.pinSize / 2 + selectionBandWidth,
												   uniforms.node.pinSpacing / 2,
												   uniforms.node.parameterPinSize / 2 + selectionBandWidth,
												   uniforms.node.parameterPinSpacing / 2,
												   uniforms.node.cornerRadius,
												   pinCount, stepper) * selectionBandShapeCorners;
	}

	
	float unbuiltBandShape = 0.0f;
	if (!isBuilt) {
		auto const unbuiltBandShapeCorners = 1.0f - stepper(1.0f - unbuiltBandWidth / uniforms.node.cornerRadius,
															cornerDistanceField);
		unbuiltBandShape = 1.0f - _nodeShapeImpl(uv, nodeSize,
												 uniforms.node.pinSize / 2 + unbuiltBandWidth,
												 uniforms.node.pinSpacing / 2,
												 uniforms.node.parameterPinSize / 2 + unbuiltBandWidth,
												 uniforms.node.parameterPinSpacing / 2,
												 uniforms.node.cornerRadius,
												 pinCount, stepper) * unbuiltBandShapeCorners;
	}
	
	return { nodeShape, selectionBandShape, unbuiltBandShape };
}

static float3 highlight(float3 color) {
	auto const t = 1 - ((1 - color)  * 0.75);
	return sqrt(t);
}

static float progressBarShade(float uvX, float buildProgress, float intensity) {
	float result = clamp(uvX - buildProgress, 0.0, 1.0) / (1 - buildProgress);
	if (buildProgress == 1) {
		result = 0;
	}
	result = sqrt(result);
	result = result * intensity;
	result += (1 - intensity);
	return mix(1, result, pow(buildProgress, 0.25));
}

fragment float4 nodeFragmentShader(NodeVSOutput in    [[ stage_in ]],
								   device worldmachine::NetworkUniforms const& uniforms
													  [[ buffer(0) ]])
{
	auto const nodeUV = in.uv;
	
	auto const shapeResult = nodeShape(nodeUV, in.nodeSize, in.pinCount, uniforms, in.buildProgress, in.isSelected, in.isBuilt);
	
	auto const nodeShape = shapeResult.nodeShape;
	if (nodeShape < 0.5) {
		metal::discard_fragment();
	}
	auto const selectionBand = shapeResult.selectionBand;
	auto const unbuiltBand = shapeResult.unbuiltBand;
	
	auto nodeBackgroundColor = uniforms.node.colors[in.category] * (1.0f + 0.0f * (1.0f - in.uv.y));
	auto const unbuiltColor = uniforms.node.unbuiltColor;
	
	nodeBackgroundColor *= progressBarShade(in.uv.x, in.buildProgress, uniforms.node.progressbarShadowIntensity);
	nodeBackgroundColor = mix(nodeBackgroundColor, uniforms.node.progressBarColor, step(nodeUV.x, in.buildProgress));
	
	auto const nodeBackgroundColorMod = mix(0, nodeBackgroundColor * nodeBackgroundColor, .85);
	nodeBackgroundColor = mix(nodeBackgroundColor, nodeBackgroundColorMod, in.uv.y);
	
	auto const interColor = mix(nodeBackgroundColor, unbuiltColor,
								unbuiltBand);
	auto const nodeColor = mix(interColor,
							   highlight(interColor),
							   selectionBand);

	return float4(nodeColor, 1);
}


