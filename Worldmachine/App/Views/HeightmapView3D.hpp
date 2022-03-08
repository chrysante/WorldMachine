#pragma once

#include "ImageDisplayView.hpp"

#include <utl/memory.hpp>
#include <mtl/mtl.hpp>

namespace worldmachine {
	
	class HeightmapRenderer3D;
	
	class HeightmapView3D: public ImageDisplayView {
	public:
		HeightmapView3D(Network*);

		void init() override;
		
	private:
		void updateImage(Image const&) override;
		void displayImage() override;
//		bool hasCachedImage() const override;
		
		/// Input
		void rightMouseDragged(MouseDragEvent) override;
		void scrollWheel(ScrollEvent) override;
		void magnify(MagnificationEvent) override;
		
		void rotate(mtl::float2 offset);
		void zoom(float offset);
		
		void debugView();
		
	private:
		utl::unique_ref<HeightmapRenderer3D> renderer;
		bool imageIsHeightmap = false;
		float rotationAngle = 0;
		float viewAngle = mtl::constants<float>::pi / 8;
		float viewDist = 3;
		float fieldOfView = mtl::constants<float>::pi / 3;
		float heightMultiplier = 0.5;
		float baseLighting = 0.5;
		bool clampHeightToZeroOneRange = true;
	};
	
}
