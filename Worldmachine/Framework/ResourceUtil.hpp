#pragma once


#include <Metal/Metal.hpp>
#include <string>
#include <filesystem>
#include <utl/functional.hpp>


namespace worldmachine {
	
	MTL::Texture* loadTextureFromFile(MTL::Device* device, std::string_view filename);
	
	std::string pathForResource(std::filesystem::path path, std::string_view extension);
	std::filesystem::path getLibraryDir();
	
	void showSaveFilePanel(utl::function<void(std::string)> completion);
	void showOpenFilePanel(utl::function<void(std::string)> completion);
	
	void performOnMainThread(utl::function<void()>);
	
}
