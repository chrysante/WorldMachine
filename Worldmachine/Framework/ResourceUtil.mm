#include "ResourceUtil.hpp"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Appkit/Appkit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include "Core/Debug.hpp"

namespace worldmachine {
	
	static NSString* toNSString(char const* cstr) {
		return [NSString stringWithUTF8String: cstr];
	}
	
	static std::string toSTDString(NSString* nsString) {
		char const* cstr = [nsString UTF8String];
		if (cstr) {
			return cstr;
		}
		return {};
	}
	
	MTL::Texture* loadTextureFromFile(MTL::Device* _device, std::filesystem::path filename, std::string_view extension) {
		NSURL *imgURL = [[NSURL alloc] initFileURLWithPath: toNSString(pathForResource(filename, extension).data())];
		
		id<MTLDevice> device = (__bridge id<MTLDevice>)_device;
		
		MTKTextureLoader* textureLoader = [[MTKTextureLoader alloc] initWithDevice:device];
		id<MTLTexture> texture = [textureLoader newTextureWithContentsOfURL:imgURL options:@{MTKTextureLoaderOptionSRGB: @true} error:NULL];
		
		MTL::Texture* result = (MTL::Texture*)CFBridgingRetain(texture);
		if (!result) {
			WM_Log(error, "Failed to load Texture {}", filename);
		}
		return result;
	}
	
	std::string pathForResource(std::filesystem::path name, std::string_view type) {
		
		NSString * path = [[NSBundle mainBundle] pathForResource:  toNSString(name.string().data()) ofType: toNSString(type.data())];
		
		return toSTDString(path);
	}
	
	std::filesystem::path getLibraryDir() {
		NSError* error;
		NSURL *fileURL = [[NSFileManager defaultManager] URLForDirectory:NSLibraryDirectory
																inDomain:NSUserDomainMask
													   appropriateForURL:nil
																  create:true
																   error:&error];
		return std::filesystem::path(fileURL.path.UTF8String) / "WorldMachine";
	}
	
	void showSaveFilePanel(utl::function<void(std::string)> completion) {
		NSSavePanel* panel = [NSSavePanel savePanel];
		
//		panel.allowedContentTypes = @[[UTType typeWithIdentifier:@"worldmachine"]];
//		panel.allowsOtherFileTypes = YES;
		
		[panel beginWithCompletionHandler:^(NSInteger result){
			if (result == NSModalResponseOK) {
				NSURL*  url = [panel URL];
				completion(toSTDString(url.path));
			}
		}];
	}
	
	void showOpenFilePanel(utl::function<void(std::string)> completion) {
		NSOpenPanel* panel = [NSOpenPanel openPanel];
		[panel setCanChooseFiles:YES];
		[panel setCanChooseDirectories:NO];
		[panel setAllowsMultipleSelection:NO];

//		panel.allowedContentTypes = @[[UTType typeWithIdentifier:@"worldmachine"]];
//		panel.allowsOtherFileTypes = YES;
		
		[panel beginWithCompletionHandler:^(NSInteger result){
			if (result == NSModalResponseOK) {
				NSURL*  url = [panel URL];
				completion(toSTDString(url.path));
			}
		}];
	}
	
	
	void performOnMainThread(utl::function<void()> f) {
		dispatch_async(dispatch_get_main_queue(), ^{
			f();
		});
	}
	
	

}
