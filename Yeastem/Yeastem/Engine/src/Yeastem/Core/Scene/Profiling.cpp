#include "yst_pch.h"
#include "Profiling.h"

YEASTEM_BEGIN

Profiler Profiler::s_Instance;

TimestampType Profiler::GetTime()
{
	return SDL_GetTicks();
}

void Profiler::PushTimeDuration(TimeStampStorageType duration, const std::string& label)
{
	Get().ProfileDurations.emplace_back(duration, label);
}

YEASTEM_END
