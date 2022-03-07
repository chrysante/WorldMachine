#import "View.hpp"

#include <imgui/imgui.h>
#include <Metal/Metal.h>
#include <Appkit/Appkit.h>
#include <CoreImage/CoreImage.h>

#include "Core/Debug.hpp"

namespace worldmachine {
	
	void displayImage(void const* texture, mtl::double2 size) {
		id<MTLTexture> nativeTexture = (__bridge id<MTLTexture>)texture;
		ImGui::Image((__bridge ImTextureID)(nativeTexture), { (float)size.x, (float)size.y });
	}
	
}


