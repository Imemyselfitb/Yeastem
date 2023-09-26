-- Note to Self: ADD LUA FILES --

workspace "Yeastem"
	architecture "x64"

	configurations
	{
		"Debug", 
		"Release", 
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include dirs relative to the root (solution) directory
IncludeDir = {}
IncludeDir["SDL2"] = "Dependancies/SDL2/include"
IncludeDir["raylib"] = "Dependancies/raylib/src"
IncludeDir["Glad"] = "Dependancies/Glad/include"

LibraryDir = {}
LibraryDir["SDL2"] = "Dependancies/SDL2/lib/"
LibraryDir["raylib"] = "Dependancies/raylib/lib/"
LibraryDir["Glad"] = "Dependancies/Glad/lib/"

include "Dependancies"

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
		"$(SolutionDir)%{IncludeDir.SDL2}", 
		"$(SolutionDir)%{IncludeDir.Glad}", 
		"$(SolutionDir)Dependancies/Glad/src"
	}

	libdirs
	{
		"$(SolutionDir)%{LibraryDir.SDL2}x64", 
		"$(SolutionDir)%{LibraryDir.Glad}x64"
	}

	links 
	{
		"SDL2.lib", 
		"SDL2main.lib", 
		"Glad.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
		kind "ConsoleApp"
		defines "YST_DEBUG"
		symbols "On"

	filter "configurations:Release"
		kind "ConsoleApp"
		defines "YST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		kind "WindowedApp"
		defines "YST_DIST"
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
		"$(SolutionDir)%{IncludeDir.raylib}"
	}

	ignoredefaultlibraries { "MSVCRT" }

	links 
	{
		"raylib"
	}

	filter "system:windows"
		cppdialect "C++17"
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
		cppdialect "C++17"
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
