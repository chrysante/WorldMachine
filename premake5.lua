-----------------------------------------------------------------------------------------
-- Workspace Worldmachine
-----------------------------------------------------------------------------------------
workspace "Worldmachine"
architecture "x86_64"
configurations {
    "Debug",
    "Release",
    "Dist"
}
filter "configurations:Debug" 
    symbols "On"
filter "configurations:Release or Dist"
    optimize "Speed"
filter {}

filter { "configurations:Debug" }
    defines { "WM_DEBUGLEVEL=2", "WM_LOGLEVEL=2" }
filter { "configurations:Release" }
    defines { "WM_DEBUGLEVEL=1", "WM_LOGLEVEL=1" }
filter { "configurations:Dist"  }
    defines { "WM_DEBUGLEVEL=0", "WM_LOGLEVEL=0" }
filter {}

includedirs {
    "worldmachine"
}

externalincludedirs {
    "Utility", "vendor"
}

filter "system:macosx" 
    cppdialect "C++20"
    staticruntime "On"
    defines { "WM_PLATFORM_MACOS" }
    externalincludedirs "vendor/Metal-cpp"
    xcodebuildsettings { 
        ["MTL_HEADER_SEARCH_PATHS"] = "../../Worldmachine ../../Utility"
    }
filter {}

targetdir("Build/Bin/%{cfg.longname}")
objdir("Build/Obj/%{cfg.longname}")

-----------------------------------------------------------------------------------------
-- Project Worldmachine
-----------------------------------------------------------------------------------------
project "Worldmachine"
location "Worldmachine/App"
kind "WindowedApp"
language "C++"

dependson "WMBuiltinNodes"

files { 
    "Worldmachine/App/**.hpp",
    "Worldmachine/App/**.cpp",
    "Worldmachine/App/Resource/**.ttf",
    "Worldmachine/App/Resource/**.png",
    "Worldmachine/App/Resource/**.json"
}
filter "system:macosx"
    files { 
        "Worldmachine/App/**.metal",
        "Worldmachine/App/Platform/macOS/MainMenu.storyboard",
        "Worldmachine/App/Platform/macOS/Worldmachine.entitlements",
        "Worldmachine/App/Platform/macOS/info.plist"
    }
    xcodebuildsettings {
        ["INFOPLIST_FILE"] = "$(SRCROOT)/Platform/macOS/info.plist"
    }
filter {}

--filter "files:Worldmachine/App/Resource/**"
--filter "files:**.png"
--    buildaction "Embed"
--filter {}
-- Copy Resources
filter "system:macosx"
    postbuildcommands {
        "{COPY} %{prj.location}/Resource/** %{cfg.targetdir}/Worldmachine.app/Contents/Resources",
        "{COPY} %{cfg.targetdir}/libWMBuiltinNodes.dylib %{cfg.targetdir}/Worldmachine.app/Contents/MacOS",
    }
filter {}

xcodebuildresources {
    "Worldmachine/App/Resource/**.ttf",
    "Worldmachine/App/Resource/**.png",
    "Worldmachine/App/Resource/**.json"
}

links { 
    "WMFramework",
    "WMCore",
    "Utility",
    "ImGui",
    "YAML"
}

filter "system:macosx"
    links {
        "AppKit.framework",
        "Metal.framework",
        "MetalKit.framework",
        "GameController.framework",
        "CoreImage.framework",
        "UniformTypeIdentifiers.framework"
    }
filter {}

--postbuildcommands {
    --"{COPY} %cfg..."
--}
-----------------------------------------------------------------------------------------
-- Project WMCore
-----------------------------------------------------------------------------------------
project "WMCore"
location "Worldmachine/Core"
kind "StaticLib"
language "C++"

files { 
    "Worldmachine/Core/**.hpp",
    "Worldmachine/Core/**.cpp"
}

-----------------------------------------------------------------------------------------
-- Project WMFramework
-----------------------------------------------------------------------------------------
project "WMFramework"
location "Worldmachine/Framework"
kind "StaticLib"
language "C++"

filter "system:macosx" 
    xcodebuildsettings { 
        ["CLANG_ENABLE_OBJC_ARC"] = "YES"
    }
filter {}

-- 
includedirs {
    "Vendor/imgui"
}

files { 
    "Worldmachine/Framework/**.hpp",
    "Worldmachine/Framework/**.cpp",
    "Worldmachine/Framework/**.mm",
}

-----------------------------------------------------------------------------------------
-- Project WMBuiltinNodes
-----------------------------------------------------------------------------------------
project "WMBuiltinNodes"
location "Worldmachine/BuiltinNodes"
kind "SharedLib"
language "C++"

links { "WMCore", "Utility", "ImGui", "YAML" }

files { 
    "Worldmachine/BuiltinNodes/**.hpp",
    "Worldmachine/BuiltinNodes/**.cpp"
}

postbuildcommands {
    "{COPY} %{cfg.targetdir}/libWMBuiltinNodes.dylib %{cfg.targetdir}/Worldmachine.app/Contents/MacOS",
}

-----------------------------------------------------------------------------------------
-- Project Utility
-----------------------------------------------------------------------------------------
project "Utility"
location "Utility"
kind "StaticLib"
language "C++"
filter "system:macosx" 
    xcodebuildsettings { 

    }
filter {}
    

files { 
    "Utility/**.hpp",
    "Utility/**.cpp",
    "Utility/**.cc",
}

-----------------------------------------------------------------------------------------
-- Project ImGui
-----------------------------------------------------------------------------------------
project "ImGui"
location "Vendor/imgui"
kind "StaticLib"
language "C++"
filter "system:macosx" 
    xcodebuildsettings { 
        ["CLANG_ENABLE_OBJC_ARC"] = "YES"
    }
filter {}

includedirs {
    "Vendor/imgui"
}

files { 
    "Vendor/imgui/imgui.cpp",
    "Vendor/imgui/imgui_demo.cpp",
    "Vendor/imgui/imgui_draw.cpp",
    "Vendor/imgui/imgui_tables.cpp",
    "Vendor/imgui/imgui_widgets.cpp",
    "Vendor/imgui/imgui.cpp",
    "Utility/**.cpp"
}

filter "system:macosx" 
    files {
        "Vendor/imgui/backends/imgui_impl_osx.mm",
        "Vendor/imgui/backends/imgui_impl_metal.mm"
    }
filter {}

-----------------------------------------------------------------------------------------
-- Project YAML
-----------------------------------------------------------------------------------------
project "YAML"
location "Vendor/yaml-cpp"
kind "StaticLib"
language "C++"
filter "system:macosx" 
    xcodebuildsettings { 

    }
filter {}
    

files {
    "Vendor/yaml-cpp/src/**.cpp"
}

-- to disable warnings from xcode to update project settings
project "*"
filter "system:macosx" 
    xcodebuildsettings { 
        ["GCC_ENABLE_FIX_AND_CONTINUE"] = "NO",
        ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
        ["CODE_SIGN_IDENTITY"] = "-",
        ["CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING"] = "YES",
        ["CLANG_WARN_BOOL_CONVERSION"] = "YES",
        ["CLANG_WARN_CONSTANT_CONVERSION"] = "YES",
        ["CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS"] = "YES",
        ["CLANG_WARN_EMPTY_BODY"] = "YES",
        ["CLANG_WARN_ENUM_CONVERSION"] = "YES",
        ["CLANG_WARN_INFINITE_RECURSION"] = "YES",
        ["CLANG_WARN_INT_CONVERSION"] = "YES",
        ["CLANG_WARN_NON_LITERAL_NULL_CONVERSION"] = "YES",
        ["CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF"] = "YES",
        ["CLANG_WARN_OBJC_LITERAL_CONVERSION"] = "YES",
        ["CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER"] = "YES",
        ["CLANG_WARN_RANGE_LOOP_ANALYSIS"] = "YES",
        ["CLANG_WARN_STRICT_PROTOTYPES"] = "YES",
        ["CLANG_WARN_SUSPICIOUS_MOVE"] = "YES",
        ["CLANG_WARN_UNREACHABLE_CODE"] = "YES",
        ["CLANG_WARN__DUPLICATE_METHOD_MATCH"] = "YES",
        ["CLANG_WARN_COMMA"] = "YES",
        ["ENABLE_TESTABILITY"] = "YES",
        ["ENABLE_STRICT_OBJC_MSGSEND"] = "YES",
        ["GCC_NO_COMMON_BLOCKS"] = "YES",
        ["GCC_WARN_UNDECLARED_SELECTOR"] = "YES",
        ["GCC_WARN_UNINITIALIZED_AUTOS"] = "YES",
        ["GCC_WARN_UNUSED_FUNCTION"] = "YES",
        ["GCC_WARN_UNUSED_VARIABLE"] = "YES",
        ["GCC_WARN_64_TO_32_BIT_CONVERSION"] = "YES",
    }
filter {}

