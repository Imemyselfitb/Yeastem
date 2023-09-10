#pragma once

#include <SDL.h>

#include <iostream>

#include "Macros.h"

YEASTEM_BEGIN



class Application
{
public:
	Application(const Application&) = delete;
	static Application& Get()
	{
		return Application::s_Instance;
	}

	static void SetupGLFlags() { Get().setupGlFlags(); }
	static void AttachWindow(SDL_Window* window) { Get().m_window = window; }
	static void CreateGLContext() { Get().createGLContext(); }

	static void Run() { return Get().run(); }

	static void DestroyWindow() { SDL_DestroyWindow(Get().m_window); }

	// Called Internally. 
	void HandleEvents(SDL_Event& m_events);
public:
	static bool s_IsRunning;

	struct PosData { int x, y; } m_WindowPosition;
	struct SizeData { int w, h; } m_WindowSize;

	SDL_Window* m_window;
	SDL_GLContext m_context;

private:
	void setupGlFlags();
	void createGLContext();

	void SetupRender();
	void render_frame();
	void run();

	Application() { s_GLInitialized = false; }
	~Application() {}

private:
	static bool s_GLInitialized;
	static Application s_Instance;
};

YEASTEM_END
