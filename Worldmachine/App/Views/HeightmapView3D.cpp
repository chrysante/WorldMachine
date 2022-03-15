#include "HeightmapView3D.hpp"

#include <imgui/imgui.h>
#include <mtl/mtl.hpp>

#include "Framework/ImGuiExt.hpp"
#include "Framework/Window.hpp"
#include "Core/Image/Image.hpp"

#include "HeightmapRenderer3D.hpp"
#include "Shaders/HeightmapRenderUniforms.hpp"

using namespace mtl;

namespace worldmachine {
	
	HeightmapView3D::HeightmapView3D(Network* network):
		View("Heightmap View 3D"),
		ImageDisplayView(network)
	{}
	
	void HeightmapView3D::init() {
		ImageDisplayView::init();
		renderer = utl::make_unique_ref<HeightmapRenderer3D>();
	}
	
	void HeightmapView3D::displayControls() {
		if (center != 0 && ImGui::Button("Reset Center")) {
			center = 0;
		}
	}
	
	void HeightmapView3D::updateImage(Image const& img) {
		if (img.dataType() == DataType::float1) {
			imageIsHeightmap = true;
			if (!img.empty()) {
				renderer->updateHeightmap(img);
			}
		}
		else {
			imageIsHeightmap = false;
		}
	}
	
	void HeightmapView3D::displayImage() {
#if 0
		debugView();
#endif
		
		if (!imageIsHeightmap) {
			return;
		}
		
		float4x4 rotation = {
			std::cos(rotationAngle), -std::sin(rotationAngle), 0, 0,
			std::sin(rotationAngle),  std::cos(rotationAngle), 0, 0,
			0,                        0,                       1, 0,
			0,                        0,                       0, 1
		};
		
		float3 eyePosition = viewDist * float3(0,
											   -std::cos(viewAngle),
											   std::sin(viewAngle)) + center;
		float4x4 view = mtl::look_at<right_handed>(eyePosition,
									 center,
									 float3(0, 0, 1));
		float const aspectRatio = this->size().x / this->size().y;
		float4x4 projection = mtl::infinite_perspective<right_handed>(fieldOfView,
														aspectRatio,
														0.01);
		
		auto const viewProjection = projection * view;
		
		HeightmapRenderUniforms uniforms{};
		uniforms.modelMatrix = mtl::transpose(rotation);
		uniforms.viewProjectionMatrix = mtl::transpose(viewProjection); // transpose to account for metal column major layout order
		uniforms.heightMultiplier = heightMultiplier;
		uniforms.baseLighting = baseLighting;
		
		if (clampHeightToZeroOneRange) {
			uniforms.heightCapMin = 0;
			uniforms.heightCapMax = 1;
		}
		else {
			uniforms.heightCapMin = FLT_MIN;
			uniforms.heightCapMax = FLT_MAX;
		}
		
		renderer->draw(this->size(), this->scaleFactor(),
					   getWindow()->appearance().windowBackgroundColor,
					   uniforms);
		
		displayTexture(renderer->renderedImage());
	}
	
	void HeightmapView3D::mouseDragged(MouseDragEvent event) {
		rotate(event.offset);
	}
	
	void HeightmapView3D::rightMouseDragged(MouseDragEvent event) {
		mouseDragged(event);
	}
	
	void HeightmapView3D::scrollWheel(ScrollEvent event) {
		if (event.isTrackpad) {
			rotate(event.offset);
		}
		else {
			zoom(- utl::signed_pow(event.offset.y, 0.666) / 100);
		}
	}
	
	bool HeightmapView3D::keyDown(KeyEvent event) {
		return false;
		float const offset = moveSpeed;
		switch (event.keyCode) {
			case KeyCode::W:
				center.y += offset;
				return true;
			case KeyCode::A:
				center.x -= offset;
				return true;
			case KeyCode::S:
				center.y -= offset;
				return true;
			case KeyCode::D:
				center.x += offset;
				return true;
				
			default:
				return false;
		}
	}
	
	void HeightmapView3D::magnify(MagnificationEvent event) {
		zoom(event.offset);
	}
	
	void HeightmapView3D::rotate(float2 offset) {
		auto const pi = mtl::constants<float>::pi;
		// arbitrary values that work well
		rotationAngle += offset.x * pi / (180 * 2);
		viewAngle -= offset.y * pi / (180 * 2);
		
		rotationAngle = utl::mod(rotationAngle, 2 * pi);
		viewAngle = std::clamp(viewAngle, -pi / 2 + 0.001f, pi / 2 - 0.001f);
	}
	
	void HeightmapView3D::zoom(float offset) {
		viewDist -= offset * viewDist;
		viewDist = std::clamp(viewDist, 0.1f, 10.0f);
	}
	
	
	void HeightmapView3D::debugView() {
		ImGui::Begin("HeightmapView3D Debug");
		ImGui::DragFloat("Max Height", &heightMultiplier, 0.001, 0, 3);
		ImGui::DragFloat("Base Lighting", &baseLighting, 0.001, 0, 1);
		ImGui::Checkbox("Clamp Height to 0-1 Range", &clampHeightToZeroOneRange);
		ImGui::Separator();
		ImGui::DragFloat("Field of View", &fieldOfView, 0.001, 0, mtl::constants<float>::pi);
		
		ImGui::Text("rotationAngle = %f", rotationAngle);
		ImGui::Text("viewAngle = %f", viewAngle);
		ImGui::Text("viewDist = %f", viewDist);
		ImGui::DragFloat("Move Speed", &moveSpeed, 0.1);
		ImGui::End();
	}
	
}
