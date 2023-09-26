outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Lua"
	location "Lua"
	kind "StaticLib"
	language "C"

	targetdir ("lib/x64/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

	files
	{
		"src/*.h",
		"src/*.c"
	}

	removefiles "src/luac.c"
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		optimize "On"
