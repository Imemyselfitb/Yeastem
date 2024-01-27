outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "EnTT"
	location "EnTT"
	kind "StaticLib"
	language "C++"

	targetdir "lib/x64/%{cfg.buildcfg}/"
	objdir "bin-int/"

	files
	{
		"include/entt.hpp"
	}

	removefiles
	{
		"src/lua.c", 
		"src/lua.h", 
		"src/luac.c"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		optimize "On"
