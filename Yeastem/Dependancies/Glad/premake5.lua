project "Glad"
	location "Glad"
	language "C"
	kind "StaticLib"

	targetdir "lib/x64/%{cfg.buildcfg}/"
	objdir "bin-int/"

	files
	{
		"src/glad.c"
	}

	includedirs
	{
		"include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
