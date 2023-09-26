#pragma once

#include <fstream>
#include <sstream>

std::string getFileContents(std::string path)
{
	std::stringstream contents;
	std::fstream input(path, std::ios::in);
	contents << input.rdbuf();
	return contents.str();
}