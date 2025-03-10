#pragma once

#define YEASTEM_BEGIN namespace Yeastem {
#define YEASTEM_END }

#define YEASTEM_ERROR(str) \
	std::cout << "YEASTEM: ERROR: " << str << "\n";
#define YEASTEM_WARNING(str) \
	std::cout << "YEASTEM: Warning: " << str << "\n";

using ObjectID = uint64_t;
