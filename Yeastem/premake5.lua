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
IncludeDir["Glad"] = "Dependancies/Glad/include"

project "Yeastem"
	location "Yeastem"
	kind "ConsoleApp"
	language "C++"

	targetdir ("$(SolutionDir)bin/" .. outputdir .. "/%{prj.name}")
	objdir ("$(SolutionDir)bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"$(SolutionDir)%{prj.name}/src", 
		"$(SolutionDir)%{IncludeDir.SDL2}", 
		"$(SolutionDir)%{IncludeDir.Glad}", 
		"$(SolutionDir)Dependancies/Glad/src"
	}

	libdirs
	{
		"$(SolutionDir)Dependancies/SDL2/lib/x64", 
		"$(SolutionDir)Dependancies/Glad/lib/x64"
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
		defines "YST_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "YST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "YST_DIST"
		optimize "On"

project "YSS Compiler"
	location "YSS Compiler"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
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
