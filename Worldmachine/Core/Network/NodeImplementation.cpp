#include "NodeImplementation.hpp"

#include <thread>
#include <random>
#include <utl/function.hpp>
#include <utl/hashmap.hpp>
#include <imgui/imgui.h>

#include "Core/Debug.hpp"
#include "Core/Registry.hpp"

#include "Node.hpp"
#include "NodeDependencyMap.hpp"

namespace worldmachine {
	
	/// MARK: - NodeImplementation
	mtl::usize2 NodeImplementation::buildResolution(BuildType type) const {
		WM_Assert(type == BuildType::preview || type == BuildType::highResolution);
		return type == BuildType::preview ? _previewBuildResolution : _highresBuildResolution;
	}
	
	/// MARK: - ImageNodeImplementation
	void ImageNodeImplementation::dynamicInit() {
		auto desc = Registry::instance().createDescriptorFromID(implementationID());
		
		_previewOutputs.clear();
		_highresOutputs.clear();
		for (auto const& i: desc.pinDescriptorArray.output) {
			_previewOutputs.push_back(Image(i.dataType()));
			_highresOutputs.push_back(Image(i.dataType()));
		}
	}
	
	void ImageNodeImplementation::clearBuildDest() {
		WM_Assert(_currentBuildType != BuildType::none);
		auto& outputs = _currentBuildType == BuildType::highResolution ?
			_highresOutputs : _previewOutputs;
		for (auto& i: outputs) {
			i.clear();
			i.resize(currentBuildResolution());
		}
	}
	
	Image const& ImageNodeImplementation::getImage(std::size_t index, BuildType type) const {
		WM_Assert(type != BuildType::none);
		if (type == BuildType::preview) {
			return _previewOutputs[index];
		}
		else {
			return _highresOutputs[index];
		}
	}
	
	Image& ImageNodeImplementation::getBuildDest(std::size_t index) {
		return utl::as_mutable(utl::as_const(*this).getBuildDest(index));
	}
	
	Image const& ImageNodeImplementation::getBuildDest(std::size_t index) const {
		WM_Assert(_currentBuildType != BuildType::none);
		if (_currentBuildType == BuildType::preview) {
			return _previewOutputs[index];
		}
		else {
			return _highresOutputs[index];
		}
	}
	
}
