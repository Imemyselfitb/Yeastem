outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Lua"
	location "Lua"
	kind "StaticLib"
	language "C"

	targetdir "lib/x64/%{cfg.buildcfg}/"
	objdir "bin-int/"

	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	optimize "On"

	files
	{
		"src/*.h",
		"src/*.c"
	}

	removefiles
	{
		"src/lua.c", 
		"src/lua.h", 
		"src/luac.c"
	}
	
	filter "system:windows"
		buildoptions { "/MP" }
