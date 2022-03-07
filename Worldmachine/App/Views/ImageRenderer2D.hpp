#pragma once

#include <Metal/Metal.hpp>
#include <mtl/mtl.hpp>

#include "Framework/MTLARCPointer.hpp"
#include "Framework/Renderer.hpp"
#include "Core/Image/Image.hpp"

namespace worldmachine {
		
	class ImageRenderer2D: public Renderer {
	public:
		ImageRenderer2D();
		
		void draw(Image const&);
		
		void clear();
		
	private:
		MTLARCPointer<MTL::RenderPipelineState> quadPipelineState;
		MTLARCPointer<MTL::Buffer>              uniformBuffer;
		
		MTLARCPointer<MTL::Texture>             imageTexture;
		MTLARCPointer<MTL::SamplerState>        samplerState;
	};

}
