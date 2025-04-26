#pragma once

YEASTEM_BEGIN

using TimestampType = uint64_t;
using TimeStampStorageType = uint32_t;

class Profiler
{
public:
	struct ProfilerDuration
	{
		TimeStampStorageType duration;
		std::string label;
	};

public:
	static Profiler& Get() { return s_Instance; }
	static TimestampType GetTime();
	static void PushTimeDuration(TimeStampStorageType duration, const std::string& label);
	static void ClearProfiles() { s_Instance.ProfileDurations.clear(); }

public:
	std::vector<ProfilerDuration> ProfileDurations;

private:
	static Profiler s_Instance;

private:
	Profiler() {};
	Profiler(const Profiler&) = delete;
	~Profiler() {}
};

struct ScopedProfileStamp
{
	std::string label;
	TimestampType startTime;

public:
	ScopedProfileStamp(const std::string& label) :startTime(Profiler::GetTime()), label(label) {}
	~ScopedProfileStamp() { Profiler::PushTimeDuration((TimeStampStorageType)(Profiler::GetTime() - startTime), label); }
};

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define YEASTEM_ScopedProfiler(name) ScopedProfileStamp TOKENPASTE2(timer_, __LINE__)(name)
#define YEASTEM_ClearProfiles() Profiler::ClearProfiles()

YEASTEM_END
