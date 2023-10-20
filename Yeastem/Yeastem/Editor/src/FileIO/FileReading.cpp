#include "yst_pch.h"

#include "FileIO.h"

YEASTEM_BEGIN

std::string readFileText(const std::string& path)
{
	std::stringstream contents;
	std::fstream input(path, std::ios::in);
	contents << input.rdbuf();
	return contents.str();
}

YEASTEM_END
