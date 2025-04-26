#pragma once

#define YEASTEM_BEGIN namespace Yeastem {
#define YEASTEM_END }

#define YEASTEM_ERROR(str) std::cout << "YEASTEM: ERROR: " << str << "\n"
#define YEASTEM_WARNING(str) std::cout << "YEASTEM: Warning: " << str << "\n"
#define YEASTEM_INFO(x) std::cout << x << "\n"

#define YEASTEM_ASSERT(condition, str)	if (!(condition)) YEASTEM_ERROR(str)

#define YEASTEM_BIND_MEMBER_FUNCTION(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

using ObjectID = uint64_t;
