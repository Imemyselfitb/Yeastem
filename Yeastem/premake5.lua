workspace "Yeastem"
	architecture "x64"

	configurations
	{
		"Debug", 
		"Release", 
		"Dist"
	}

	startproject "YeastemEditor"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include dirs relative to the root (solution) directory
IncludeDir = {}
IncludeDir["SDL3"] = "Dependancies/SDL3/include"
IncludeDir["stb_image"] = "Dependancies/stb_image/include"
IncludeDir["Glad"] = "Dependancies/Glad/include"
IncludeDir["Lua"] = "Dependancies/Lua/src"
IncludeDir["EnTT"] = "Dependancies/EnTT/include"
IncludeDir["ImGui"] = "Dependancies/ImGui/src"

include "Dependancies"

EngineLocation = "Yeastem/Engine"

project "YeastemEngine"
	kind "StaticLib"
	language "C++"
	location (EngineLocation)

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchsource (EngineLocation .. "/src/yst_pch.cpp")

	defines { "SDL_MAIN_HANDLED", "SDL_STATIC" }

	files
	{
		EngineLocation .. "/src/**.h",
		EngineLocation .. "/src/**.cpp"
	}

	includedirs
	{
		EngineLocation .. "/src", 
		"%{IncludeDir.SDL3}", 
		"%{IncludeDir.stb_image}", 
		"%{IncludeDir.Lua}", 
		"%{IncludeDir.Glad}", 
		"Dependancies/Glad/src", 
		"%{IncludeDir.EnTT}", 
		"%{IncludeDir.ImGui}", 
		"%{IncludeDir.ImGui}/backends"
	}

	links
	{
		"SDL3",
		"stb_image", 
		"Glad", 
		"Lua", 
		"ImGui"
	}

	ignoredefaultlibraries { "MSVCRT" }

	----- ----- ----- ----- ----- ----- OS's ----- ----- ----- ----- ----- ----- 
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		defines "YST_PLATFORM_WINDOWS"
		buildoptions { "/MP" }
		pchheader "yst_pch.h"

	filter "system:linux"
		cppdialect "C++latest"
		staticruntime "On"
		defines "YST_PLATFORM_LINUX"
		pchheader (EngineLocation .. "/src/yst_pch.h")

	----- ----- ----- ----- ----- ----- CONFIG's ----- ----- ----- ----- ----- ----- 
	filter "configurations:Debug"
		defines "YST_CONFIG_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "YST_CONFIG_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "YST_CONFIG_DIST"
		optimize "On"

project "YeastemEditor"
	local m_Location = "Yeastem/Editor"
	location ( m_Location )

	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		m_Location .. "/src",
		EngineLocation .. "/src",
		"%{IncludeDir.SDL3}", 
		"%{IncludeDir.stb_image}", 
		"%{IncludeDir.Lua}", 
		"%{IncludeDir.Glad}", 
		"Dependancies/Glad/src", 
		"%{IncludeDir.EnTT}", 
		"%{IncludeDir.ImGui}", 
		"%{IncludeDir.ImGui}/backends"
	}

	links
	{
		"Yeastem Engine",
		"SDL3",
		"ImGui"
	}

	ignoredefaultlibraries { "MSVCRT" }

	defines
	{
		"SDL_MAIN_HANDLED",
		"SDL_STATIC"
	}

	----- ----- ----- ----- ----- ----- OS's ----- ----- ----- ----- ----- ----- 
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		defines "YST_PLATFORM_WINDOWS"
		buildoptions { "/MP" }

	filter "system:linux"
		cppdialect "C++latest"
		staticruntime "On"
		defines "YST_PLATFORM_LINUX"

	----- ----- ----- ----- ----- ----- CONFIG's ----- ----- ----- ----- ----- ----- 
	filter "configurations:Debug"
		kind "ConsoleApp"
		defines "YST_CONFIG_DEBUG"
		symbols "On"

	filter "configurations:Release"
		kind "ConsoleApp"
		defines "YST_CONFIG_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		kind "WindowedApp"
		defines "YST_CONFIG_DIST"
		optimize "On"

project "YeastemRuntime"
	local m_Location = "Yeastem/Runtime"
	location ( m_Location)
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		m_Location .. "/src", 
		"%{IncludeDir.SDL3}", 
		"%{IncludeDir.Glad}", 
		"Dependancies/Glad/src"
	}

	libdirs
	{
		"Dependancies/SDL3/lib/x64/%{cfg.buildcfg}", 
		"Dependancies/Glad/lib/x64/%{cfg.buildcfg}"
	}

	links
	{
		"SDL3",
		"Glad"
	}

	ignoredefaultlibraries { "MSVCRT" }

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		buildoptions { "/MP" }

	filter "configurations:Debug"
		defines "YST_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "YST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "YST_DIST"
		optimize "On"

project "YSSCompiler"
	local m_Location = "Yeastem/Compiler"
	location ( m_Location )

	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		m_Location .. "/src/**.ys",
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		m_Location .. "/src", 
	}

	defines
	{
		"SDL_MAIN_HANDLED",
		"SDL_STATIC"
	}

	ignoredefaultlibraries { "MSVCRT" }

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		buildoptions { "/MP" }

	filter "configurations:Debug"
		defines "YST_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "YST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "YST_DIST"
		optimize "On"
