#pragma once

#include <Metal/Metal.hpp>
#include <mtl/mtl.hpp>

#include "Framework/Renderer.hpp"
#include "Framework/MTLARCPointer.hpp"
#include "Core/Image/Image.hpp"

#include "Shaders/HeightmapRenderUniforms.hpp"

namespace worldmachine {
	
	class HeightmapRenderer3D: public Renderer {
	public:
		HeightmapRenderer3D();
		
		void updateHeightmap(ImageView<float const>);
		void draw(mtl::double2 targetSize, mtl::double2 scaleFactor,
				  mtl::float4 clearColor,
				  HeightmapRenderUniforms);
		
	private:
		void updateSize(mtl::usize2);
		
	private:
		mtl::usize2 vertexResolution = -1;
		
		MTLARCPointer<MTL::RenderPipelineState> heightmapPipelineState;
		MTLARCPointer<MTL::Buffer>              uniformBuffer;
		
		MTLARCPointer<MTL::Texture>             heightTexture;
		MTLARCPointer<MTL::Buffer>              heightmapVertexBuffer;
		MTLARCPointer<MTL::Buffer>              heightmapIndexBuffer;
		MTLARCPointer<MTL::SamplerState>        samplerState;
		
		MTLARCPointer<MTL::DepthStencilState>   depthStencilStateOpaque;
	};

}
