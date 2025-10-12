#include "Yeastem.h"

#include "Statistics.h"

#include "Yeastem/Core/Scene/Profiling.h"
#include <imgui.h>
#include <chrono>

YEASTEM_BEGIN

static uint32_t FPS = 0;
void calculateFPS()
{
	static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
	static uint32_t fps;
	fps++;

	std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::high_resolution_clock::now();
	if (std::chrono::template duration_cast<std::chrono::seconds>(currentTime - oldTime) >= std::chrono::seconds{ 1 })
	{
		oldTime = currentTime;
		FPS = fps;
		fps = 0;
	}
}

void ShowStatisticsPanel(bool& isOpen, float deltaTime)
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	ImGui::Begin("Stats", &isOpen);
	if (isOpen)
	{
		ImGui::TextWrapped("  There were %d scopes! DeltaTime = %.3fms", Profiler::Get().ProfileDurations.size(), deltaTime);
		ImGui::TextWrapped("  FPS: %d", FPS);
		for (const Profiler::ProfilerDuration& d : Profiler::Get().ProfileDurations)
		{
			ImGui::Text((d.label + "     " + std::to_string(d.duration) + "ns").c_str());
		}
	}
	ImGui::End();
}

YEASTEM_END
