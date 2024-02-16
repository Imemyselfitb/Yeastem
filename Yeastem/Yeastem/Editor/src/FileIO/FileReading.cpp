#include "yst_pch.h"

#include "FileIO.h"

YEASTEM_BEGIN

namespace FileIO
{
	std::string readFileText(const std::string& path)
	{
		std::stringstream contents;
		std::fstream input(path, std::ios::in);
		contents << input.rdbuf();

		if (!contents.good())
		{
			YEASTEM_ERROR("Failed to Load File `" << path << "`!");
			return "";
		}

		return contents.str();
	}

	bool checkIfExists(const std::string& path)
	{
		std::ifstream f(path.c_str());
		return f.good();
	}
}

YEASTEM_END
