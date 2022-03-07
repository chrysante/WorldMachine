#pragma once

#include "Framework/ShaderMath.hpp"
#include "Framework/Typography/TextRenderData.hpp"
#include "Core/Network/SharedNetworkTypes.hpp"

namespace worldmachine {
	
	struct PortalParameters {
		metal::float2 size;
		metal::float2 position;
		float scale; 
	};
	
	struct SelectionParameters {
		metal::float4 color = metal::float4(0, .8, 1, .3);
		metal::float4 borderColor = metal::float4(0, 1, 1, .6);
		float borderSize = 1;
	};
	
	struct LabelRenderData {
		metal::float2 worldPosition;
		float size;
		float pixelRange;
	};
	
	struct NetworkUniforms {
		metal::float4x4 viewProjectionMatrix;
		
		NodeParameters node;
		EdgeParameters edge;
		PortalParameters portal;
		SelectionParameters selection;
		float textSize;
		metal::float2 screenScaleFactor;
		float nodeCount;
		metal::float2 shadowOffset;
		FontMetrics fontMetrics;
		metal::float4 backgroundColor;
		metal::float4 edgeColor;
		metal::float4 labelColor;
		metal::float4 lineColor;
	};
	
	
	
}

