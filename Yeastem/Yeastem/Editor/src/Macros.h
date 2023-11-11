#pragma once

#define YEASTEM_BEGIN namespace Yeastem {
#define YEASTEM_END }

#define YEASTEM_ERROR(str) \
	std::cout << "YEASTEM: ERROR: " << str << std::endl;
#define YEASTEM_WARNING(str) \
	std::cout << "YEASTEM: Warning: " << str << std::endl;

