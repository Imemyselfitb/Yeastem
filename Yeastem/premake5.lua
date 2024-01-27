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
IncludeDir["stb_image"] = "Dependancies/stb_image/include"
IncludeDir["Glad"] = "Dependancies/Glad/include"
IncludeDir["Lua"] = "Dependancies/Lua/src"
IncludeDir["EnTT"] = "Dependancies/EnTT/include"
IncludeDir["ImGui"] = "Dependancies/ImGui/src"

LibraryDir = {}
LibraryDir["SDL2"] = "Dependancies/SDL2/lib/"

include "Dependancies"

project "Yeastem Editor"
	local m_Location = "Yeastem/Editor"
	location ( m_Location )

	language "C++"

	targetdir ("$(SolutionDir)bin/" .. outputdir .. "/%{prj.name}")
	objdir ("$(SolutionDir)bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "yst_pch.h"
	pchsource (m_Location .. "/src/yst_pch.cpp")

	files
	{
		m_Location .. "/src/**.h",
		m_Location .. "/src/**.cpp"
	}

	includedirs
	{
		"$(SolutionDir)" .. m_Location .. "/src", 
		"$(SolutionDir)%{IncludeDir.SDL2}", 
		"$(SolutionDir)%{IncludeDir.stb_image}", 
		"$(SolutionDir)%{IncludeDir.Lua}", 
		"$(SolutionDir)%{IncludeDir.Glad}", 
		"$(SolutionDir)Dependancies/Glad/src", 
		"$(SolutionDir)%{IncludeDir.EnTT}", 
		"$(SolutionDir)%{IncludeDir.ImGui}", 
		"$(SolutionDir)%{IncludeDir.ImGui}/backends"
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

	filter "system:windows"
		cppdialect "C++latest"
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
		"SDL2main.lib", 
		"Dependancies/Glad"
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
