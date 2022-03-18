#pragma once

#include <Metal/Metal.hpp>
#include "Framework/Platform/MacOS/MTLARCPointer.hpp"
#include <string>
#include <mtl/mtl.hpp>
#include "Vertex2D.hpp"

namespace worldmachine {

	[[nodiscard]]
	MTLARCPointer<MTL::RenderPipelineDescriptor> makeDefaultPipelineStateDescriptor(MTL::Library*,
																					std::string_view vertexFunction,
																					std::string_view fragmentFunction,
																					bool usesDepth,
																					bool translucent);
	
	
	inline void fillBuffer(MTL::Buffer* buffer, void const* data, std::size_t size) {
		std::memcpy(buffer->contents(), data, size);
		buffer->didModifyRange(NS::Range(0, size));
	}
	
	template <typename T>
	void fillBuffer(MTL::Buffer* buffer, T const& t) {
		fillBuffer(buffer, &t, sizeof t);
	}
	
	class Renderer {
	public:
		Renderer();
		
		[[nodiscard]]
		MTLARCPointer<MTL::RenderPipelineDescriptor> defaultPipelineDescriptor(MTL::Library*,
																			   std::string_view vertexShader,
																			   std::string_view fragmentShader) const;
		[[nodiscard]]
		MTLARCPointer<MTL::RenderPipelineDescriptor> translucencyPipelineDescriptor(MTL::Library*,
																					std::string_view vertexShader,
																					std::string_view fragmentShader) const;
		
	
		MTL::Texture* renderedImage() { return renderTexture.get(); }
		mtl::usize2   renderedImageSize() const { return renderTextureSize; }
		
		[[nodiscard]] MTL::Texture* createTexture2D(MTL::PixelFormat format, mtl::usize2 size, bool mipmapped = true) const;
		void generateMipmaps(MTL::Texture*);
		
		
	protected:
		void updateRenderTargets(mtl::usize2);
		void createRenderTargets(mtl::usize2);
		MTL::Buffer const* quadVertexBuffer() const { return _quadVertexBuffer.get(); }
		
	protected:
		MTLARCPointer<MTL::Device>       device;
		MTLARCPointer<MTL::CommandQueue> commandQueue;
		MTLARCPointer<MTL::Texture>      renderTexture;
		MTLARCPointer<MTL::Texture>      depthTexture;
		mtl::usize2                      renderTextureSize = 0;
		
		bool useDepthBuffer = false;
		int  MSAA = 1;
		
	private:
		MTLARCPointer<MTL::Buffer>       _quadVertexBuffer;
	};
	
}
