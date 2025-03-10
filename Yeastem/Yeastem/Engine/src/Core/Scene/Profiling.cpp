#include "yst_pch.h"
#include "Profiling.h"

YEASTEM_BEGIN

Profiler Profiler::s_Instance;

/*
YEASTEM_TimeStampStorageType ConvertToTimestamp(YEASTEM_TimestampType t)
{ return std::chrono::time_point_cast<std::chrono::milliseconds>(t).time_since_epoch().count(); }

YEASTEM_TimestampType Profiler::GetTime()
{ return std::chrono::high_resolution_clock::now(); }
*/

YEASTEM_TimeStampStorageType ConvertToTimestamp(YEASTEM_TimestampType t) { return (YEASTEM_TimeStampStorageType)t; }
YEASTEM_TimestampType Profiler::GetTime() { return SDL_GetTicks64(); }

void Profiler::PushTimeDuration(YEASTEM_TimeStampStorageType duration, const std::string& label)
{ Get().ProfileDurations.emplace_back(duration, label); }

YEASTEM_END
