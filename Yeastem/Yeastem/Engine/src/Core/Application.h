#pragma once

#include "OpenGL/Include.h"
#include "ImGuiAPI.h"

#include "ResourceManager.h"
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
	static void AttachWindow(SDL_Window* window) { Get().m_Window = window; }
	static void CreateGLContext() { Get().createGLContext(); }
	static void InitImGui() { Get().initImGui(); }
	static ResourceManager& GetResourceManager() { return Get().m_ResourceManager; }

	static void Run() { return Get().run(); }

	static void DestroyWindow() { SDL_DestroyWindow(Get().m_Window); }

public:
	Scene CurrentScene;
	bool IsRunning = false;

private:
	void handleEvents(SDL_Event& m_events);

	void setupGlFlags() const;
	void createGLContext();

	void initImGui();

	void SetupRender();
	void process_frame(float deltaTime);
	void run();

private:
	Application() { m_GLInitialized = false; }
	~Application() { if (m_Window != nullptr) DestroyWindow(); }

private:
	struct WindowPosData { int x, y; } m_WindowPosition = { 0, 0 };
	struct WindowSizeData { int w, h; } m_WindowSize = { 0, 0 };

	SDL_Window* m_Window = nullptr;
	SDL_Renderer* s_Renderer = nullptr;
	SDL_GLContext m_Context = 0;

private:
	uint64_t m_CurrentTime = 0;
	bool m_GLInitialized = false;
	ResourceManager m_ResourceManager;

	std::unique_ptr<Yeastem::ImGuiAPI> m_ImGuiLayer;

private:
	static Application s_Instance;
};

YEASTEM_END
