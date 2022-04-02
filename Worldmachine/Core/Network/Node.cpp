#include "Node.hpp"

namespace worldmachine {
	
	NodeParameters defaultNodeParameters() {
		return {
			.cornerRadius              = 20,
			.selectionBandWidth        = 5,
			.unbuiltBandWidth          = 2.5,
			.pinSize                   = 20,
			.pinSpacing                = 30,
			.parameterPinSize          = 12.5,
			.parameterPinSpacing       = 22.5,
			.outerShadowSize           = 50.0,
			.innerShadowSize           = 30,
			.shadowIntensity           = 0.75,
			.unbuiltColor              = mtl::float3(1, 0, 0)
		};
	}
	
	mtl::float2 nodeSize(NodeParameters nodeParams, PinCount<float> pinCount) {
		float const x = [&]{
			auto const iCount = std::max(pinCount.parameterInput, 3.0f);
			return iCount * nodeParams.parameterPinSpacing + 2.0f * nodeParams.cornerRadius;
		}();
		float const y = [&]{
			auto const iCount = std::max({ pinCount.input, pinCount.output, 1.0f });
			return iCount * nodeParams.pinSpacing + 2.0f * nodeParams.cornerRadius;
		}();
		return { x, y };
	}
	
	PinCount<> calculatePinCount(NodeDescriptor const& desc) {
		auto _count = [&](PinKind kind) -> std::uint8_t {
			auto const& pins = desc.pinDescriptorArray.get(kind);
			return pins.size();
		};
		return {
			_count(PinKind::input),
			_count(PinKind::output),
			_count(PinKind::parameterInput),
			_count(PinKind::maskInput)
		};
	}
	
}
