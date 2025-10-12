outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "EnTT"
	location "EnTT"
	kind "None"
	language "C++"

	targetdir "lib/x64/%{cfg.buildcfg}/"
	objdir "bin-int/"

	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"
	optimize "On"

	files
	{
		"include/entt.hpp"
	}
