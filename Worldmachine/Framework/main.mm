// Dear ImGui: standalone example application for OSX + Metal.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include <mutex>
#include <memory>
#include <utl/format.hpp>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_metal.h>
#include <imgui/backends/imgui_impl_osx.h>

#include "Core/Debug.hpp"
#include "Window.hpp"
#include "ResourceUtil.hpp"
#include "Event.hpp"

static worldmachine::KeyCode translateKeyCode(int keyCode) {
	switch (keyCode) {
	   case kVK_ANSI_A:              return worldmachine::KeyCode::A;
	   case kVK_ANSI_S:              return worldmachine::KeyCode::S;
	   case kVK_ANSI_D:              return worldmachine::KeyCode::D;
	   case kVK_ANSI_F:              return worldmachine::KeyCode::F;
	   case kVK_ANSI_H:              return worldmachine::KeyCode::H;
	   case kVK_ANSI_G:              return worldmachine::KeyCode::G;
	   case kVK_ANSI_Z:              return worldmachine::KeyCode::Z;
	   case kVK_ANSI_X:              return worldmachine::KeyCode::X;
	   case kVK_ANSI_C:              return worldmachine::KeyCode::C;
	   case kVK_ANSI_V:              return worldmachine::KeyCode::V;
	   case kVK_ANSI_B:              return worldmachine::KeyCode::B;
	   case kVK_ANSI_Q:              return worldmachine::KeyCode::Q;
	   case kVK_ANSI_W:              return worldmachine::KeyCode::W;
	   case kVK_ANSI_E:              return worldmachine::KeyCode::E;
	   case kVK_ANSI_R:              return worldmachine::KeyCode::R;
	   case kVK_ANSI_Y:              return worldmachine::KeyCode::Y;
	   case kVK_ANSI_T:              return worldmachine::KeyCode::T;
	   case kVK_ANSI_1:              return worldmachine::KeyCode::_1;
	   case kVK_ANSI_2:              return worldmachine::KeyCode::_2;
	   case kVK_ANSI_3:              return worldmachine::KeyCode::_3;
	   case kVK_ANSI_4:              return worldmachine::KeyCode::_4;
	   case kVK_ANSI_6:              return worldmachine::KeyCode::_6;
	   case kVK_ANSI_5:              return worldmachine::KeyCode::_5;
	   case kVK_ANSI_Equal:          return worldmachine::KeyCode::equal;
	   case kVK_ANSI_9:              return worldmachine::KeyCode::_9;
	   case kVK_ANSI_7:              return worldmachine::KeyCode::_7;
	   case kVK_ANSI_Minus:          return worldmachine::KeyCode::minus;
	   case kVK_ANSI_8:              return worldmachine::KeyCode::_8;
	   case kVK_ANSI_0:              return worldmachine::KeyCode::_0;
	   case kVK_ANSI_RightBracket:   return worldmachine::KeyCode::rightBracket;
	   case kVK_ANSI_O:              return worldmachine::KeyCode::O;
	   case kVK_ANSI_U:              return worldmachine::KeyCode::U;
	   case kVK_ANSI_LeftBracket:    return worldmachine::KeyCode::leftBracket;
	   case kVK_ANSI_I:              return worldmachine::KeyCode::I;
	   case kVK_ANSI_P:              return worldmachine::KeyCode::P;
	   case kVK_ANSI_L:              return worldmachine::KeyCode::L;
	   case kVK_ANSI_J:              return worldmachine::KeyCode::J;
	   case kVK_ANSI_Quote:          return worldmachine::KeyCode::apostrophe;
	   case kVK_ANSI_K:              return worldmachine::KeyCode::K;
	   case kVK_ANSI_Semicolon:      return worldmachine::KeyCode::semicolon;
	   case kVK_ANSI_Backslash:      return worldmachine::KeyCode::backslash;
	   case kVK_ANSI_Comma:          return worldmachine::KeyCode::comma;
	   case kVK_ANSI_Slash:          return worldmachine::KeyCode::slash;
	   case kVK_ANSI_N:              return worldmachine::KeyCode::N;
	   case kVK_ANSI_M:              return worldmachine::KeyCode::M;
	   case kVK_ANSI_Period:         return worldmachine::KeyCode::period;
	   case kVK_ANSI_Grave:          return worldmachine::KeyCode::graveAccent;
	   case kVK_ANSI_KeypadDecimal:  return worldmachine::KeyCode::keypadDecimal;
	   case kVK_ANSI_KeypadMultiply: return worldmachine::KeyCode::keypadMultiply;
	   case kVK_ANSI_KeypadPlus:     return worldmachine::KeyCode::keypadAdd;
	   case kVK_ANSI_KeypadClear:    return worldmachine::KeyCode::numLock;
	   case kVK_ANSI_KeypadDivide:   return worldmachine::KeyCode::keypadDivide;
	   case kVK_ANSI_KeypadEnter:    return worldmachine::KeyCode::keypadEnter;
	   case kVK_ANSI_KeypadMinus:    return worldmachine::KeyCode::keypadSubtract;
	   case kVK_ANSI_KeypadEquals:   return worldmachine::KeyCode::keypadEqual;
	   case kVK_ANSI_Keypad0:        return worldmachine::KeyCode::keypad0;
	   case kVK_ANSI_Keypad1:        return worldmachine::KeyCode::keypad1;
	   case kVK_ANSI_Keypad2:        return worldmachine::KeyCode::keypad2;
	   case kVK_ANSI_Keypad3:        return worldmachine::KeyCode::keypad3;
	   case kVK_ANSI_Keypad4:        return worldmachine::KeyCode::keypad4;
	   case kVK_ANSI_Keypad5:        return worldmachine::KeyCode::keypad5;
	   case kVK_ANSI_Keypad6:        return worldmachine::KeyCode::keypad6;
	   case kVK_ANSI_Keypad7:        return worldmachine::KeyCode::keypad7;
	   case kVK_ANSI_Keypad8:        return worldmachine::KeyCode::keypad8;
	   case kVK_ANSI_Keypad9:        return worldmachine::KeyCode::keypad9;
	   case kVK_Return:              return worldmachine::KeyCode::enter;
	   case kVK_Tab:                 return worldmachine::KeyCode::tab;
	   case kVK_Space:               return worldmachine::KeyCode::space;
	   case kVK_Delete:              return worldmachine::KeyCode::backspace;
	   case kVK_Escape:              return worldmachine::KeyCode::escape;
	   case kVK_CapsLock:            return worldmachine::KeyCode::capsLock;
	   case kVK_Control:             return worldmachine::KeyCode::leftCtrl;
	   case kVK_Shift:               return worldmachine::KeyCode::leftShift;
	   case kVK_Option:              return worldmachine::KeyCode::leftAlt;
	   case kVK_Command:             return worldmachine::KeyCode::leftSuper;
	   case kVK_RightControl:        return worldmachine::KeyCode::rightCtrl;
	   case kVK_RightShift:          return worldmachine::KeyCode::rightShift;
	   case kVK_RightOption:         return worldmachine::KeyCode::rightAlt;
	   case kVK_RightCommand:        return worldmachine::KeyCode::rightSuper;
	   case kVK_F5:                  return worldmachine::KeyCode::F5;
	   case kVK_F6:                  return worldmachine::KeyCode::F6;
	   case kVK_F7:                  return worldmachine::KeyCode::F7;
	   case kVK_F3:                  return worldmachine::KeyCode::F3;
	   case kVK_F8:                  return worldmachine::KeyCode::F8;
	   case kVK_F9:                  return worldmachine::KeyCode::F9;
	   case kVK_F11:                 return worldmachine::KeyCode::F11;
	   case kVK_F13:                 return worldmachine::KeyCode::printScreen;
	   case kVK_F10:                 return worldmachine::KeyCode::F10;
	   case 0x6E:                    return worldmachine::KeyCode::menu;
	   case kVK_F12:                 return worldmachine::KeyCode::F12;
	   case kVK_Help:                return worldmachine::KeyCode::insert;
	   case kVK_Home:                return worldmachine::KeyCode::home;
	   case kVK_PageUp:              return worldmachine::KeyCode::pageUp;
	   case kVK_ForwardDelete:       return worldmachine::KeyCode::delete_;
	   case kVK_F4:                  return worldmachine::KeyCode::F4;
	   case kVK_End:                 return worldmachine::KeyCode::end;
	   case kVK_F2:                  return worldmachine::KeyCode::F2;
	   case kVK_PageDown:            return worldmachine::KeyCode::pageDown;
	   case kVK_F1:                  return worldmachine::KeyCode::F1;
	   case kVK_LeftArrow:           return worldmachine::KeyCode::leftArrow;
	   case kVK_RightArrow:          return worldmachine::KeyCode::rightArrow;
	   case kVK_DownArrow:           return worldmachine::KeyCode::downArrow;
	   case kVK_UpArrow:             return worldmachine::KeyCode::upArrow;
	   default:                      return worldmachine::KeyCode::none;
   }
}

static bool isTrackpadScroll(NSEvent* event) {
	return event.phase != 0 || event.momentumPhase != 0;
}

static worldmachine::Event toEvent(NSEvent* event) {
	return {
		.modifierFlags    = (worldmachine::EventModifierFlags)((unsigned)event.modifierFlags >> 16)
	};
}

static worldmachine::KeyEvent toKeyEvent(NSEvent* event) {
	return {
		toEvent(event),
		.keyCode = translateKeyCode(event.keyCode)
	};
}

static worldmachine::MouseEvent toMouseEvent(NSEvent* event) {
	return {
		toEvent(event),
		.locationInWindow = { event.locationInWindow.x, event.locationInWindow.y },
		.locationInView   = {  }
	};
}
static worldmachine::MouseDownEvent toMouseDownEvent(NSEvent* event) {
	return {
		toMouseEvent(event),
		.clickCount = event.clickCount
	};
}

static worldmachine::MouseUpEvent toMouseUpEvent(NSEvent* event) {
	return {
		toMouseEvent(event)
	};
}

static worldmachine::MouseDragEvent toMouseDraggedEvent(NSEvent* event) {
	return {
		toMouseEvent(event),
		.offset = { event.deltaX, event.deltaY }
	};
}

static worldmachine::ScrollEvent toScrollEvent(NSEvent* event) {
	return {
		toMouseEvent(event),
		.offset     = { event.scrollingDeltaX, event.scrollingDeltaY },
		.isTrackpad = isTrackpadScroll(event)
	};
}

static worldmachine::MagnificationEvent toMagnificationEvent(NSEvent* event) {
	return {
		toMouseEvent(event),
		.offset = event.magnification
	};
}

static worldmachine::MouseMoveEvent toMouseMoveEvent(NSEvent* event) {
	return {
		toMouseEvent(event),
		.offset = { event.deltaX, event.deltaY }
	};
}

//-----------------------------------------------------------------------------------
// MARK: WMKeyEventResponder
//-----------------------------------------------------------------------------------

@interface WMKeyEventResponder: NSView<NSTextInputClient>
@property (readwrite) utl::function<bool(NSEvent* _Nonnull)> keyEventHandler;
@end

@implementation WMKeyEventResponder
{
	float _posX;
	float _posY;
	NSRect _imeRect;
}

- (void)setImePosX:(float)posX imePosY:(float)posY
{
	_posX = posX;
	_posY = posY;
}

- (void)updateImePosWithView:(NSView *)view
{
	NSWindow *window = view.window;
	if (!window)
		return;
	NSRect contentRect = [window contentRectForFrameRect:window.frame];
	NSRect rect = NSMakeRect(_posX, contentRect.size.height - _posY, 0, 0);
	_imeRect = [window convertRectToScreen:rect];
}

- (void)viewDidMoveToWindow
{
	// Ensure self is a first responder to receive the input events.
	[self.window makeFirstResponder:self];
}

- (void)keyDown:(NSEvent*)event
{
	if (!self.keyEventHandler(event))
		[super keyDown:event];

	// Call to the macOS input manager system.
	[self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent*)event
{
	if (!self.keyEventHandler(event))
		[super keyUp:event];
}

- (void)insertText:(id)aString replacementRange:(NSRange)replacementRange
{
	ImGuiIO& io = ImGui::GetIO();

	NSString* characters;
	if ([aString isKindOfClass:[NSAttributedString class]])
		characters = [aString string];
	else
		characters = (NSString*)aString;

	io.AddInputCharactersUTF8(characters.UTF8String);
}

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (void)doCommandBySelector:(SEL)myselector
{
}

- (nullable NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange
{
	return nil;
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point
{
	return 0;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange
{
	return _imeRect;
}

- (BOOL)hasMarkedText
{
	return NO;
}

- (NSRange)markedRange
{
	return NSMakeRange(NSNotFound, 0);
}

- (NSRange)selectedRange
{
	return NSMakeRange(NSNotFound, 0);
}

- (void)setMarkedText:(nonnull id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange
{
}

- (void)unmarkText
{
}

- (nonnull NSArray<NSAttributedStringKey>*)validAttributesForMarkedText
{
	return @[];
}

@end

//-----------------------------------------------------------------------------------
// MARK: AppViewController
//-----------------------------------------------------------------------------------

//ImGuiContext* g_imguiContext;

struct AppData {
	ImFont* defaultFont;
	worldmachine::Window* window;
	int dirty;
	std::string iniFilename;
};

@interface AppViewController : NSViewController
@end

@interface AppViewController () <MTKViewDelegate>
@property (nonatomic, readonly) MTKView *mtkView;
@property (nonatomic, readonly) MTKView *mtkView2;
@property (nonatomic, strong) id <MTLDevice> device;
@property (nonatomic, strong) id <MTLCommandQueue> commandQueue;
@property ImGuiContext* imguiContext;

@property AppData appData;
@property WMKeyEventResponder* keyEventResponder;
@property NSTextInputContext*  inputContext;
@end

@implementation AppViewController

extern worldmachine::Window* createMainWindow();

-(instancetype)initWithNibName:(nullable NSString *)nibNameOrNil bundle:(nullable NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    self.device = MTLCreateSystemDefaultDevice();
    self.commandQueue = [_device newCommandQueue];

	// so we refresh a couple of times on startup to give imgui
	// some time to layout itself
	_appData.dirty = 10;
	
    if (!self.device)
    {
        NSLog(@"Metal is not supported");
        abort();
    }
	
	bool const DESTROY_INI_FILE = false;
	if (DESTROY_INI_FILE)  {
	FILE* f = std::fopen("imgui.ini", "w");
	if (f) { std::fclose(f); }
	else { throw; }
	}
    
	
	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
	_imguiContext = ImGui::CreateContext();

	{
		// Set .ini filename
		_appData.iniFilename = worldmachine::getLibraryDir() / "worldmachine-imgui.ini";
		_imguiContext->IO.IniFilename = _appData.iniFilename.data();
	}
	
	
	
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	
    // Setup Dear ImGui style
//    ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();
//    ImGui::StyleColorsClassic();

    // Setup Renderer backend
    ImGui_ImplMetal_Init(_device);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	
	float const fontSize = 16;
	float const dpiScale = 2;
	
	if (auto font = io.Fonts->AddFontFromFileTTF(worldmachine::pathForResource("SFPro-Regular-Roman", "ttf").c_str(), fontSize * dpiScale)) {
		_appData.defaultFont = font;
	}
	else {
		throw;
	}
	io.Fonts->AddFontFromFileTTF(worldmachine::pathForResource("SFMono-Regular-Roman", "ttf").c_str(), fontSize * dpiScale);
	io.Fonts->Build();
	
	io.FontGlobalScale = 1.0 / dpiScale;
	
	ImGui_ImplMetal_CreateFontsTexture(_device);
	
    return self;
}

static bool WM_G_HasDeallocated = false;

-(void)dealloc
{
	WM_G_HasDeallocated = true;
	delete self.appData.window;
	ImGui::DestroyContext(_imguiContext);
}

-(MTKView *)mtkView
{
    return (MTKView *)self.view;
}

-(void)loadView
{
    self.view = [[MTKView alloc] initWithFrame:CGRectMake(0, 0, 1200, 720)];
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    self.mtkView.device                = self.device;
    self.mtkView.delegate              = self;
	self.mtkView.paused                = true;
	self.mtkView.enableSetNeedsDisplay = true;
	
	// Add a tracking area in order to receive mouse events whenever the mouse is within the bounds of our view
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:NSZeroRect
                                                                options:NSTrackingMouseMoved | NSTrackingInVisibleRect | NSTrackingActiveAlways
                                                                  owner:self
                                                               userInfo:nil];
    [self.view addTrackingArea:trackingArea];
	
	ImGui_ImplOSX_Init(self.view);
	
	self.keyEventResponder = [[WMKeyEventResponder alloc] initWithFrame: NSZeroRect];
	self.keyEventResponder.keyEventHandler = [](NSEvent* _Nonnull event) -> bool { return false; };
	
	__weak AppViewController* weakSelf = self;
	
	auto keyEventHandler = [=](NSEvent* _Nonnull event) -> bool {
		[weakSelf invalidate : 1];
		if (ImGui::GetIO().WantCaptureKeyboard) {
			return ImGui_ImplOSX_HandleEvent(event, nil);
		}
		using namespace worldmachine::internal;
		auto result =  _windowHandleKeyEvent(*weakSelf.appData.window,
											 toKeyEvent(event),
											 event.type == NSEventTypeKeyDown);
		return result;
	};
	
	self.keyEventResponder.keyEventHandler = keyEventHandler;
	self.inputContext = [[NSTextInputContext alloc] initWithClient: self.keyEventResponder];
	[self.view addSubview: self.keyEventResponder];
	
	// Some events do not raise callbacks of AppView in some circumstances (for example when CMD key is held down).
	// This monitor taps into global event stream and captures these events.
	NSEventMask eventMask = NSEventMaskFlagsChanged;
	[NSEvent addLocalMonitorForEventsMatchingMask:eventMask handler:^NSEvent * _Nullable(NSEvent *event)
	{
		keyEventHandler(event);
		return event;
	}];
	
	_appData.window = createMainWindow();
	worldmachine::internal::_windowSetInvalidateFn(*_appData.window, [=](std::size_t dirty){
		dispatch_async(dispatch_get_main_queue(), ^{
			[weakSelf invalidate : (int)dirty];
		});
	});
}

-(void)drawInMTKView:(MTKView*)view
{
	if (_appData.dirty) {
		/**
		 This is a dirty hack to ensure the view gets drawn again after recieving an event
		 */
		[self invalidate: 0];
		--(_appData.dirty);
	}
	
    ImGuiIO& io = ImGui::GetIO();
	
    io.DisplaySize.x = view.bounds.size.width;
    io.DisplaySize.y = view.bounds.size.height;

    CGFloat framebufferScale = view.window.screen.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;
    io.DisplayFramebufferScale = ImVec2(framebufferScale, framebufferScale);

    io.DeltaTime = 1 / float(view.preferredFramesPerSecond ?: 60);

    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];

    MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;
    if (renderPassDescriptor == nil)
    {
        [commandBuffer commit];
		return;
    }

    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDescriptor);
    ImGui_ImplOSX_NewFrame(view);
    ImGui::NewFrame();
	assert(self.appData.defaultFont);
	ImGui::PushFont(self.appData.defaultFont);
	
	// My Code
	worldmachine::internal::_doDisplayWindow(*_appData.window);
	

	ImGui::PopFont();
	
    // Rendering
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
	
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
    id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    [renderEncoder pushDebugGroup:@"Dear ImGui rendering"];
    ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);
    [renderEncoder popDebugGroup];
    [renderEncoder endEncoding];

	// Present
    [commandBuffer presentDrawable:view.currentDrawable];
    [commandBuffer commit];
}

-(void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{
}

//-----------------------------------------------------------------------------------
// MARK: Input processing
//-----------------------------------------------------------------------------------

-(bool)imguiHandleEvent:(NSEvent *)event {
	return ImGui_ImplOSX_HandleEvent(event, self.view);
}

-(void)invalidate:(int)dirty {
	[self.mtkView setNeedsDisplay:true];
	self->_appData.dirty = std::max(self->_appData.dirty, dirty);
}

-(void)handleEvent:(worldmachine::EventUnion)eventUnion withEvent: (NSEvent *)event {
	// invalidate two frames because with just one frame imgui will sometimes behave weird
	[self invalidate : 2];
	[self imguiHandleEvent: event];
	using namespace worldmachine::internal;
	_windowHandleMouseEvent(*_appData.window, eventUnion);
}

// Forward Mouse events to Dear ImGui OSX backend.
-(void)mouseDown:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::mouseDown,
		.mouseDownEvent = toMouseDownEvent(event)
	} withEvent:event];
}
-(void)rightMouseDown:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::rightMouseDown,
		.mouseDownEvent = toMouseDownEvent(event)
	} withEvent:event];
}
-(void)otherMouseDown:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::otherMouseDown,
		.mouseDownEvent = toMouseDownEvent(event)
	} withEvent:event];
}
-(void)mouseUp:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::mouseUp,
		.mouseUpEvent = toMouseUpEvent(event)
	} withEvent:event];
}
-(void)rightMouseUp:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::rightMouseUp,
		.mouseUpEvent = toMouseUpEvent(event)
	} withEvent:event];
}
-(void)otherMouseUp:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::otherMouseUp,
		.mouseUpEvent = toMouseUpEvent(event)
	} withEvent:event];
}
-(void)mouseMoved:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::mouseMoved,
		.mouseMoveEvent = toMouseMoveEvent(event)
	} withEvent:event];
}
-(void)rightMouseMoved:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::rightMouseMoved,
		.mouseMoveEvent = toMouseMoveEvent(event)
	} withEvent:event];
}
-(void)otherMouseMoved:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::otherMouseMoved,
		.mouseMoveEvent = toMouseMoveEvent(event)
	} withEvent:event];
}
-(void)mouseDragged:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::mouseDragged,
		.mouseDragEvent = toMouseDraggedEvent(event)
	} withEvent:event];
}
-(void)rightMouseDragged:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::rightMouseDragged,
		.mouseDragEvent = toMouseDraggedEvent(event)
	} withEvent:event];
}
-(void)otherMouseDragged:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::otherMouseDragged,
		.mouseDragEvent = toMouseDraggedEvent(event)
	} withEvent:event];
}
-(void)scrollWheel:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::scrollWheel,
		.scrollEvent = toScrollEvent(event)
	} withEvent:event];
}
-(void)magnifyWithEvent:(NSEvent *)event {
	using namespace worldmachine;
	[self handleEvent:EventUnion{
		.type = worldmachine::EventType::magnify,
		.magnificationEvent = toMagnificationEvent(event)
	} withEvent:event];
}


@end

//-----------------------------------------------------------------------------------
// AppDelegate
//-----------------------------------------------------------------------------------
#include <dlfcn.h>
#include <stdio.h>
@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, strong) NSWindow *window;
@end

@implementation AppDelegate

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	
}

-(instancetype)init
{
    if (self = [super init])
    {
        NSViewController *rootViewController = [[AppViewController alloc] initWithNibName:nil bundle:nil];
        self.window = [[NSWindow alloc] initWithContentRect:NSZeroRect
                                                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
        self.window.contentViewController = rootViewController;
        [self.window orderFront:self];
        [self.window center];
//        [self.window becomeKeyWindow];
    }
    return self;
}

@end

//-----------------------------------------------------------------------------------
// Application main() function
//-----------------------------------------------------------------------------------

int main(int argc, const char * argv[])
{
	return NSApplicationMain(argc, argv);
	WM_Assert(WM_G_HasDeallocated);
}

