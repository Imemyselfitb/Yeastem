#pragma once

YEASTEM_BEGIN

namespace FileIO
{
	std::string readFileText(const std::string& path);
	bool checkIfExists(const std::string& path);
}

YEASTEM_END
