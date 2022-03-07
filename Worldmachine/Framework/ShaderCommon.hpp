#pragma once



inline metal::float4 toColor(float val) { return metal::float4(val, val, val, 1); }
inline metal::float4 toColor(metal::float2 val) { return metal::float4(val, 0, 1); }
inline metal::float4 toColor(metal::float3 val) { return metal::float4(val, 1); }

namespace worldmachine {
	
	struct VertexOut {
		metal::float4 clipSpacePosition [[ position ]];
		metal::float2 textureCoordinates;
	};
	
}

// MARK: - roundedRectangle
/*
 * t in [0, max]
 * 2 * slopeSize <= max
 */
inline float stepGradientSlopeSize(float t, float max, float slopeSize) {
	if (t < slopeSize) {
		return t / (2.0f *  slopeSize);
	}
	else if (t < max - slopeSize) {
		return 0.5f;
	}
	else {
		return (t - max) / (2.0f * slopeSize) + 1.0f;
	}
}

inline metal::float2 stepGradientSlopeSize(float2 t, float2 max, float2 slopeSize) {
	return float2(stepGradientSlopeSize(t.x, max.x, slopeSize.x),
				  stepGradientSlopeSize(t.y, max.y, slopeSize.y));
}

inline float roundedRectangleDistanceField(float2 uv, float2 rectSize, float cornerRadius) {
	auto const cornerUV = stepGradientSlopeSize(uv * rectSize, rectSize, cornerRadius);

	auto const distanceField = 2.0f * metal::length(cornerUV - 0.5);

	return distanceField;
}



struct Stepper {
	Stepper(float scale): diff(0.02f / scale) {}
	
	float operator()(float edge, float x) const {
		return metal::smoothstep(edge - diff, edge, x);
	}
	
private:
	float diff;
};


inline metal::float2 relativeSize(metal::float2 size) {
	auto const ratio = size.x / size.y;
	return float2(metal::max(1.f, ratio), metal::max(1.f, 1.f / ratio));
}

inline metal::float2 normalizeCoordinates(metal::float2 coordinates, metal::float2 size) {
	return coordinates * relativeSize(size);
}

inline float isInZeroOneRange(float f) {
	return f < 0.0f || f > 1.0f ? 0.0f : 1.0f;
}

inline float isInZeroOneRange(metal::float2 f) {
	return isInZeroOneRange(f.x) * isInZeroOneRange(f.y);
}

inline metal::float2 rotate90DegreesCounterClockwise(metal::float2 p) {
	return { -p.y, p.x };
}
