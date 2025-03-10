workspace "Yeastem"
	architecture "x64"

	configurations
	{
		"Debug", 
		"Release", 
		"Dist"
	}

	startproject "Yeastem Editor"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include dirs relative to the root (solution) directory
IncludeDir = {}
IncludeDir["SDL2"] = "Dependancies/SDL2/include"
IncludeDir["stb_image"] = "Dependancies/stb_image/include"
IncludeDir["Glad"] = "Dependancies/Glad/include"
IncludeDir["Lua"] = "Dependancies/Lua/src"
IncludeDir["EnTT"] = "Dependancies/EnTT/include"
IncludeDir["ImGui"] = "Dependancies/ImGui/src"
IncludeDir["glm"] = "Dependancies/glm/include"

LibraryDir = {}
LibraryDir["SDL2"] = "Dependancies/SDL2/lib/"

include "Dependancies"

EngineLocation = "Yeastem/Engine"

project "Yeastem Engine"
	kind "StaticLib"
	
	location ( EngineLocation )

	language "C++"

	targetdir ("$(SolutionDir)bin/" .. outputdir .. "/%{prj.name}")
	objdir ("$(SolutionDir)bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "yst_pch.h"
	pchsource (EngineLocation .. "/src/yst_pch.cpp")

	files
	{
		EngineLocation .. "/src/**.h",
		EngineLocation .. "/src/**.cpp"
	}

	includedirs
	{
		"$(SolutionDir)" .. EngineLocation .. "/src", 
		"$(SolutionDir)%{IncludeDir.SDL2}", 
		"$(SolutionDir)%{IncludeDir.stb_image}", 
		"$(SolutionDir)%{IncludeDir.Lua}", 
		"$(SolutionDir)%{IncludeDir.Glad}", 
		"$(SolutionDir)Dependancies/Glad/src", 
		"$(SolutionDir)%{IncludeDir.EnTT}", 
		"$(SolutionDir)%{IncludeDir.ImGui}", 
		"$(SolutionDir)%{IncludeDir.ImGui}/backends",
		"$(SolutionDir)%{IncludeDir.glm}"
	}
	 
	libdirs
	{
		"$(SolutionDir)%{LibraryDir.SDL2}x64"
	}

	links 
	{
		-- SDL Libraries
		"SDL2.lib", 
		"SDL2main.lib", 
		-- SDL Projects
		"stb_image", 

		-- Projects
		"Glad", 
		"Lua", 
		"ImGui"
	}

	----- ----- ----- ----- ----- ----- OS's ----- ----- ----- ----- ----- ----- 
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"
		defines "YST_PLATFORM_WINDOWS"

	filter "system:linux"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"
		defines "YST_PLATFORM_LINUX"

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

project "Yeastem Editor"
	local m_Location = "Yeastem/Editor"
	location ( m_Location )

	language "C++"

	targetdir ("$(SolutionDir)bin/" .. outputdir .. "/%{prj.name}")
	objdir ("$(SolutionDir)bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		"$(SolutionDir)" .. m_Location .. "/src",
		"$(SolutionDir)" .. EngineLocation .. "/src",
		"$(SolutionDir)Yeastem",
		"$(SolutionDir)%{IncludeDir.SDL2}", 
		"$(SolutionDir)%{IncludeDir.stb_image}", 
		"$(SolutionDir)%{IncludeDir.Lua}", 
		"$(SolutionDir)%{IncludeDir.Glad}", 
		"$(SolutionDir)Dependancies/Glad/src", 
		"$(SolutionDir)%{IncludeDir.EnTT}", 
		"$(SolutionDir)%{IncludeDir.ImGui}", 
		"$(SolutionDir)%{IncludeDir.ImGui}/backends",
		"$(SolutionDir)%{IncludeDir.glm}"
	}

	links 
	{
		"Yeastem Engine"
	}

	----- ----- ----- ----- ----- ----- OS's ----- ----- ----- ----- ----- ----- 
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"
		defines "YST_PLATFORM_WINDOWS"

	filter "system:linux"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"
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

project "Yeastem Runtime"
	local m_Location = "Yeastem/Runtime"
	location ( m_Location)
	kind "ConsoleApp"
	language "C++"

	targetdir ("$(SolutionDir)bin/" .. outputdir .. "/%{prj.name}")
	objdir ("$(SolutionDir)bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		"$(SolutionDir)" .. m_Location .. "/src", 
		"$(SolutionDir)%{IncludeDir.SDL2}", 
		"$(SolutionDir)%{IncludeDir.Glad}", 
		"$(SolutionDir)Dependancies/Glad/src"
	}

	libdirs
	{
		"$(SolutionDir)%{LibraryDir.SDL2}x64"
	}

	links 
	{
		"SDL2.lib", 
		"SDL2main.lib"
	}

	ignoredefaultlibraries { "MSVCRT" }

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		defines "YST_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "YST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "YST_DIST"
		optimize "On"

project "YSS Compiler"
	local m_Location = "Yeastem/Compiler"
	location ( m_Location )

	kind "ConsoleApp"
	language "C++"

	targetdir ("$(SolutionDir)bin/" .. outputdir .. "/%{prj.name}")
	objdir ("$(SolutionDir)bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		m_Location .. "/src/**.ys",
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		"$(SolutionDir)" .. m_Location .. "/src", 
	}

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		defines "YST_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "YST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "YST_DIST"
		optimize "On"
