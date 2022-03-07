#pragma once


#include <utl/operator_definition_macros.hpp>
#include "Framework/ShaderMath.hpp"

#if WORLDMACHINE(CPP)

#include <utl/bit.hpp>
#include <utl/array.hpp>
#include <iosfwd>
#include "Core/Debug.hpp"
#include "Core/Network/Pin.hpp"

#endif // WORLDMACHINE(CPP)


namespace worldmachine {
	
	WORLDMACHINE(CONSTANT) unsigned nodeNameMaxRenderSize = 32;

	enum struct NodeCategory: int {
		generator = 0,
		filter,
		natural,
		output,
		parameter,
		count
	};
#if WORLDMACHINE(CPP)
	inline std::ostream& operator<<(std::ostream& str, NodeCategory c) {
		WM_BoundsCheck((int)c, 0, (int)NodeCategory::count);
		char const* values[] = {
			"Generator",
			"Filter",
			"Natural",
			"Output",
			"Parameter"
		};
		return str << values[(int)c];
	}
#endif
	
	struct NodeParameters {
		float cornerRadius;
		float selectionBandWidth;
		float unbuiltBandWidth;
		float pinSize;
		float pinSpacing;
		float parameterPinSize;
		float parameterPinSpacing;
		float outerShadowSize;
		float innerShadowSize;
		float shadowIntensity;
		float progressbarShadowIntensity;
		metal::float3 colors[(int)NodeCategory::count];
		metal::float3 unbuiltColor;
		metal::float3 progressBarColor;
	};
	
	template <typename T = uint8_t>
	struct PinCount {
		T input;
		T output;
		T parameterInput;
		T maskInput;
		
#if WORLDMACHINE(CPP)
		
		T get(PinKind ifKind) const {
			static_assert(utl::to_underlying(PinKind::input) == 0);
			static_assert(utl::to_underlying(PinKind::output) == 1);
			static_assert(utl::to_underlying(PinKind::parameterInput) == 2);
			static_assert(utl::to_underlying(PinKind::maskInput) == 3);
			
			return utl::bit_cast<utl::array<T, 4>>(*this)[utl::to_underlying(ifKind)];
		}
		
#endif // WORLDMACHINE(CPP)
		
		template <typename U>
		constexpr operator PinCount<U>() const {
			return {
				static_cast<U>(input),
				static_cast<U>(output),
				static_cast<U>(parameterInput),
				static_cast<U>(maskInput)
			};
		}
	};
	
	enum struct NodeFlags {
		none                 = 0,
		selected             = 1 << 0,
		building             = 1 << 1,
		built                = 1 << 2,
		previewBuilt         = 1 << 3,
	};
	UTL_ENUM_BITWISE_OPERATORS_DEF(NodeFlags);
	
	struct EdgeParameters {
		float width;
	};
	
	struct EdgeProxy {
		metal::float2 begin, end;
		float beginIndex, endIndex;
	};
	
	
}
