#include "yst_pch.h"

#include "Application.h"

#include "OpenGL/Include.h"

#include "Yeastem/LuaAPI/Lua.h"

YEASTEM_BEGIN

static Application s_Instance;

Application& Application::Get()
{
	return s_Instance;
}

void Application::Init(const WindowInfoData& windowInfo)
{
	// Set Version of Glad (4.6)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// Use Core Profile, and Double Buffer
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Use a Depth size of 24
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if (!SDL_Init(SDL_INIT_VIDEO))
		YEASTEM_ERROR("SDL could not be initialized: " << SDL_GetError());

	m_Window = SDL_CreateWindow(windowInfo.Title, windowInfo.Width, windowInfo.Height, windowInfo.Flags);
	m_Context = SDL_GL_CreateContext(m_Window);
	SDL_GL_MakeCurrent(m_Window, m_Context);

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	s_Renderer = SDL_CreateRenderer(m_Window, nullptr);
	m_ImGuiLayer.Init(m_Window, m_Context);
	m_CurrentScene.Init(m_ResourceManager);
}

void Application::HandleEvents(SDL_Event& currentEvent)
{
	YEASTEM_ScopedProfiler("APPLICATION: Events");
	while (SDL_PollEvent(&currentEvent))
	{
		ImGui_ImplSDL3_ProcessEvent(&currentEvent);
		if (currentEvent.type == SDL_EVENT_QUIT || (
			currentEvent.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
			currentEvent.window.windowID == SDL_GetWindowID(m_Window)))
		{
			m_IsRunning = false;
			return;
		}

		m_CurrentScene.GetEventHandler().Dispatch(currentEvent);
	}
}

int seed = 0;
int pseudo_rand()
{
	seed = (seed - 1) * (seed ^ 0x02468) * 3;
	return seed;
}

void Application::RenderScene()
{
}

void Application::UpdateScene()
{
	
}

void Application::Run()
{
	m_IsRunning = true;

	SDL_Event m_events;
	float deltaTime = 0.0f;

	while (m_IsRunning)
	{
		YEASTEM_ClearProfiles();
		YEASTEM_ScopedProfiler("APPLICATION: Frame");

		glViewport(0, 0, (uint32_t)m_CurrentScene.SceneSize.x, (uint32_t)m_CurrentScene.SceneSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGuiNextFrame();

		// Event Handling
		{
			YEASTEM_ScopedProfiler("APPLICATION: Events");
			HandleEvents(m_events);
		}

		// Updating Frame
		{
			YEASTEM_ScopedProfiler("SCENE: Update");
			uint64_t newTime = SDL_GetTicks();
			uint64_t deltaTime = newTime - m_CurrentTime;
			deltaTime = SDL_min(deltaTime, 8); //  Max 8ms
			m_CurrentTime = newTime;

			const uint8_t frameDeltaTime = 16;
			m_AccumulatedTime += (uint8_t)deltaTime;
			while (m_AccumulatedTime > frameDeltaTime)
			{
				m_CurrentScene.Update(frameDeltaTime / 1000.0f);
				m_AccumulatedTime -= frameDeltaTime;
			}
		}

		// Update All Panels
		m_ImGuiLayer.Update();

		// Render Scene
		{
			YEASTEM_ScopedProfiler("SCENE: Render");
			m_CurrentScene.Render(m_ResourceManager);
		}

		SDL_GL_SwapWindow(m_Window);
		m_ImGuiLayer.BackupFrame();
	}

	m_ImGuiLayer.Destroy();
	SDL_GL_DestroyContext(m_Context);
	SDL_DestroyRenderer(s_Renderer);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

YEASTEM_END	