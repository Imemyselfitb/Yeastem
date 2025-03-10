#pragma once

YEASTEM_BEGIN

// #define YEASTEM_TimestampType std::chrono::time_point<std::chrono::steady_clock>
// #define YEASTEM_TimeStampStorageType long long

#define YEASTEM_TimestampType uint64_t
#define YEASTEM_TimeStampStorageType uint32_t

YEASTEM_TimeStampStorageType ConvertToTimestamp(YEASTEM_TimestampType t);

class Profiler
{
public:
	struct ProfilerDuration
	{
		YEASTEM_TimeStampStorageType duration;
		std::string label;
	};

public:
	static Profiler& Get() { return s_Instance; }
	static YEASTEM_TimestampType GetTime();
	static void PushTimeDuration(YEASTEM_TimeStampStorageType duration, const std::string& label);
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
	YEASTEM_TimestampType startTime;
public:
	ScopedProfileStamp(const std::string& label) :startTime(Profiler::GetTime()), label(label) {}
	~ScopedProfileStamp() {  Profiler::PushTimeDuration(ConvertToTimestamp(Profiler::GetTime()) - ConvertToTimestamp(startTime), label); }
};

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define YEASTEM_ScopedProfiler(name) ScopedProfileStamp TOKENPASTE2(timer_, __LINE__)(name)

YEASTEM_END
