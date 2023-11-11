project "stb_image"
	location "stb_image"
	kind "StaticLib"
	language "C++"

	targetdir "lib/x64/%{cfg.buildcfg}/"
	objdir "bin-int/"

	files
	{
		"src/stb_image.cpp", 
		"include/stb_image.h"
	}
	
	includedirs
	{
		"include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		optimize "On"
