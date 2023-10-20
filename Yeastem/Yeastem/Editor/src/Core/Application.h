#pragma once

#include "OpenGL/Include.h"

#include "ImGuiAPI.h"

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
	static void InitImGui() { Get().initImGui(); }

	static void Run() { return Get().run(); }

	static void DestroyWindow() { SDL_DestroyWindow(Get().m_window); }

	// Called Internally. 
	void HandleEvents(SDL_Event& m_events) const;
public:
	static bool s_IsRunning;

public:
	struct PosData { int x, y; } m_WindowPosition;
	struct SizeData { int w, h; } m_WindowSize;

	SDL_Window* m_window;
	SDL_GLContext m_context;

private:
	void setupGlFlags() const;
	void createGLContext();

	void initImGui();

	void SetupRender();
	void render_frame();
	void run();

private:
	Application() { s_GLInitialized = false; }
	~Application() { if (m_window != nullptr) this->DestroyWindow(); }

private:
	std::unique_ptr<Yeastem::ImGuiAPI> m_ImGuiLayer;
	Yeastem::Renderer m_Renderer;

private:
	static bool s_GLInitialized;
	static Application s_Instance;
};

YEASTEM_END
