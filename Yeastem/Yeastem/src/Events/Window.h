#pragma once

#include <SDL.h>

#include <unordered_map>

#include "Macros.h"

YEASTEM_BEGIN

struct WindowData
{
	int left, top;
	int width, height;
};

class WindowEvent
{
public:
	WindowEvent(const WindowEvent&) = delete;

	static WindowEvent& Get() { return s_Instance; }

	static void HandleEvent(const SDL_WindowEvent& e) { Get().handle_events(e); }

	// Defined in Application
	static void Repaint();

private:
	WindowEvent() {}
	~WindowEvent() {}

	void handle_events(const SDL_WindowEvent& e);

private:
	static WindowEvent s_Instance;

	std::unordered_map<int, WindowData> AllWindowsData;
};

YEASTEM_END
