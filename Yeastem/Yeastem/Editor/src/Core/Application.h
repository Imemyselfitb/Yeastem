#pragma once

#include "OpenGL/Include.h"
#include "ImGuiAPI.h"

#include "Scene/Scene.h"

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
	static SDL_Renderer* s_Renderer;

public:
	struct PosData { int x, y; } m_WindowPosition = {0, 0};
	struct SizeData { int w, h; } m_WindowSize = {0, 0};

	SDL_Window* m_window = nullptr;
	SDL_GLContext m_context = 0;

private:
	void setupGlFlags() const;
	void createGLContext();

	void initImGui();

	void SetupRender();
	void process_frame(float deltaTime);
	void run();

private:
	Application() { s_GLInitialized = false; }
	~Application() { if (m_window != nullptr) this->DestroyWindow(); }

public:
	static std::unique_ptr<Yeastem::ImGuiAPI> s_ImGuiLayer;

public:
	uint64_t CurrentTime = 0;
	static Scene CurrentScene;

private:
	static bool s_GLInitialized;
	static Application s_Instance;
};

YEASTEM_END
