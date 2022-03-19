#define UTL_DEFER_MACROS

#include "Framework/Appearance.hpp"

#include "Core/Debug.hpp"
#include <utl/scope_guard.hpp>

namespace worldmachine {
	
	Appearance::Type internal::getCurrentAppearanceType() {
		return Appearance::Type::dark;
	}
	
	Appearance Appearance::getCurrent() {
		Appearance result{};
		
		result.type = internal::getCurrentAppearanceType();
		
		result.labelColor = {};
		result.secondaryLabelColor = {};
		result.tertiaryLabelColor = {};
		result.quaternaryLabelColor = {};
		result.textColor = {};
		result.placeholderTextColor = {};
		result.selectedTextColor = {};
		result.textBackgroundColor = {};
		result.selectedTextBackgroundColor = {};
		result.keyboardFocusIndicatorColor = {};
		result.unemphasizedSelectedTextColor = {};
		result.unemphasizedSelectedTextBackgroundColor = {};
		result.linkColor = {};
		result.separatorColor = {};
		result.selectedContentBackgroundColor = {};
		result.unemphasizedSelectedContentBackgroundColor = {};
		result.selectedMenuItemTextColor = {};
		result.gridColor = {};
		result.headerTextColor = {};
		result.alternatingContentBackgroundColors[0{};
		result.alternatingContentBackgroundColors[1{};
		result.controlAccentColor = {};
		result.controlColor = {};
		result.controlBackgroundColor = {};
		result.controlTextColor = {};
		result.disabledControlTextColor = {};
		result.currentControlTint = {};
		result.selectedControlColor = {};
		result.selectedControlTextColor = {};
		result.alternateSelectedControlTextColor = {};
		result.scrubberTexturedBackground = {};
		result.windowBackgroundColor = {};
		result.windowFrameTextColor = {};
		result.underPageBackgroundColor = {};
		result.findHighlightColor = {};
		result.highlightColor = {};
		result.shadowColor = {};
		
		switch (result.type) {
			case Type::light:
				result.logTextColors[utl::log2((unsigned)utl::log_level::trace)]    = mtl::colors<mtl::double4>::hex(0x0000007F);
				result.logTextColors[utl::log2((unsigned)utl::log_level::info)]     = mtl::colors<mtl::double4>::hex(0x03CC4FFF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::debug)]    = mtl::colors<mtl::double4>::hex(0x9200FFFF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::warning)]  = mtl::colors<mtl::double4>::hex(0xD89400FF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::error)]    = mtl::colors<mtl::double4>::hex(0xFF0000FF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::fatal)]    = mtl::colors<mtl::double4>::hex(0xFF0000FF);
				break;
				
			case Type::dark:
				result.logTextColors[utl::log2((unsigned)utl::log_level::trace)]    = mtl::colors<mtl::double4>::hex(0xFFFFFF7F);
				result.logTextColors[utl::log2((unsigned)utl::log_level::info)]     = mtl::colors<mtl::double4>::hex(0x28E570FF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::debug)]    = mtl::colors<mtl::double4>::hex(0xC16EFFFF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::warning)]  = mtl::colors<mtl::double4>::hex(0xFFD65FFF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::error)]    = mtl::colors<mtl::double4>::hex(0xFF4545FF);
				result.logTextColors[utl::log2((unsigned)utl::log_level::fatal)]    = mtl::colors<mtl::double4>::hex(0xFF4545FF);
				break;
				
			default:
				// ???
				break;
				
		}
		
		
		
		return result;
	}
	
	
	
}
