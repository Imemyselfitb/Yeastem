#pragma once

#include "OpenGL/Include.h"
#include "ImGuiAPI.h"

#include "ResourceManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

YEASTEM_BEGIN

struct WindowInfoData
{
	const char* Title = nullptr;
	Vector2i Size = { 0, 0 };
	int Flags = 0;
};

class Application
{
public:
	Application() {}
	Application(const Application&) = delete;
	static Application& Get();

	void Init(const WindowInfoData& windowInfo);
	void Run();
	void Close() { Get().m_IsRunning = false; }

	void SetCurrentScene(Ref<Scene>& scene) { m_CurrentScene = scene; }

	ResourceManager& GetResourceManager() { return m_ResourceManager; }
	ImGuiAPI& GetImGuiLayer() { return m_ImGuiLayer; }
	Ref<Scene>& GetCurrentScene() { return m_CurrentScene; }
	SDL_Window* GetWindow() { return m_Window; }
	EventHandler& GetEventHandler() { return m_EventHandler; }

private:
	void HandleEvents(SDL_Event& m_events);

private:
	Ref<Scene> m_CurrentScene;
	ResourceManager m_ResourceManager;
	ImGuiAPI m_ImGuiLayer;
	EventHandler m_EventHandler;

	SDL_Window* m_Window = nullptr;
	SDL_Renderer* s_Renderer = nullptr;
	SDL_GLContext m_Context = 0;

	uint8_t m_AccumulatedTime = 0;
	uint64_t m_CurrentTime = 0;
	bool m_IsRunning = false;
	bool m_GLInitialized = false;
};

YEASTEM_END
