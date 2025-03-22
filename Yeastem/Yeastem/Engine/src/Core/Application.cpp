#include "yst_pch.h"

#include "Application.h"

#include "OpenGL/Include.h"

#include "LuaAPI/Lua.h"

YEASTEM_BEGIN

Application Application::s_Instance;

void Application::setupGlFlags() const
{
	if (m_GLInitialized) return;

	// Set Version of Glad (4.6)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// Use core Profile, and Double Buffer
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Use a Depth size of 24
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

void Application::createGLContext()
{
	m_Context = SDL_GL_CreateContext(m_Window);

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	m_GLInitialized = true;
}

void Application::init(const WindowInfoData& windowInfo)
{
	SetupGLFlags();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		YEASTEM_ERROR("SDL could not be initialized: " << SDL_GetError());

	m_Window = SDL_CreateWindow(windowInfo.Title, windowInfo.X, windowInfo.Y, windowInfo.Width, windowInfo.Height, windowInfo.Flags);

	CreateGLContext();
	InitImGui();

	CurrentScene.Init(m_ResourceManager);
}

void Application::handleEvents(SDL_Event& m_events)
{
	YEASTEM_ScopedProfiler("APPLICATION: Events");
	while (SDL_PollEvent(&m_events))
	{
		ImGui_ImplSDL2_ProcessEvent(&m_events);
		if (m_events.type == SDL_QUIT)
		{
			IsRunning = false;
			return;
		}

		if (m_events.type == SDL_WINDOWEVENT && m_events.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			if (m_events.window.windowID == SDL_GetWindowID(m_Window))
			{
				IsRunning = false;
				return;
			}
		}

		CurrentScene.ProcessEvent(m_events);
	}
}

void Application::SetupRender()
{
	SDL_GetWindowSize(m_Window, &m_WindowSize.w, &m_WindowSize.h);

	m_ImGuiLayer->AddWindow([&](bool& isVisible) {
		if (!isVisible)
			IsRunning = false;

		ImGuiWindowFlags flags = 0;
		flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		//flags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("Stage", &isVisible, flags);

		ImVec2 WindowSize = ImGui::GetWindowSize();
		
		WindowSize.y -= 35;

		//std::cout << "PLUH " << CurrentScene.DoesFrameBufferExist();
		if (!CurrentScene.DoesFrameBufferExist())
		{
			CurrentScene.CreateFrameBuffer((uint32_t)WindowSize.x, (uint32_t)WindowSize.y);
			CurrentScene.SceneSize = WindowSize;
		}

		uint32_t glColourAttachmentId = CurrentScene.GetFrameBufferColourAttachmentID();
		ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(glColourAttachmentId)), WindowSize, { 0, 1 }, { 1, 0 });

		if (CurrentScene.SceneSize.x != WindowSize.x || CurrentScene.SceneSize.y != WindowSize.y)
		{
			CurrentScene.SceneSize = WindowSize;
			CurrentScene.ShouldRefreshFrameBuffer = true;
		}
		
		ImGui::End();
	});
}

int seed = 0;
int pseudo_rand()
{
	seed = (seed - 1) * (seed ^ 0x02468) * 3;
	return seed;
}

void Application::process_frame(float deltaTime)
{
	SDL_GetWindowPosition(m_Window, &m_WindowPosition.x, &m_WindowPosition.y);
	SDL_GetWindowSize(m_Window, &m_WindowSize.w, &m_WindowSize.h);

	glViewport(0, 0, (uint32_t)CurrentScene.SceneSize.x, (uint32_t)CurrentScene.SceneSize.y);
	glClear(GL_COLOR_BUFFER_BIT);

	{
		YEASTEM_ScopedProfiler("SCENE: Update");
		CurrentScene.Update(deltaTime);
	}

	{
		YEASTEM_ScopedProfiler("SCENE: Render");
		CurrentScene.Render(m_ResourceManager);
	}
}

void Application::initImGui()
{
	m_ImGuiLayer = std::make_unique<Yeastem::ImGuiAPI>(m_Window, &m_Context);
}

int FPS = 0;
void calcFPS() {
	static std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now();
	static int fps; fps++;


	std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - oldTime) >= std::chrono::seconds{ 1 })
	{
		oldTime = currentTime;
		FPS = fps;
		fps = 0;
	}
}

void Application::run()
{
	s_Renderer = SDL_CreateRenderer(m_Window, -1, 0);

	IsRunning = true;

	m_ImGuiLayer->Init();
	m_ImGuiLayer->AddWindow([&](bool&) {
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {
					std::cout << "New" << '\n';
				}
				if (ImGui::MenuItem("Open")) { std::cout << "Open" << '\n'; }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { std::cout << "Save" << '\n'; }
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) { std::cout << "Save As" << '\n'; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Cut", "Ctrl+X")) { std::cout << "Cut" << '\n'; }
				if (ImGui::MenuItem("Copy", "Ctrl+C")) { std::cout << "Copy" << '\n'; }
				if (ImGui::MenuItem("Paste", "Ctrl+V")) { std::cout << "Paste" << '\n'; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Quit")) { IsRunning = false; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	});

	// -- RUN SCRIPTS -- //
	SetupRender();

	SDL_Event m_events;
	unsigned long long previous_time = 0;
	float accumulated_time = 0;
	float deltaTime = 0.0f;
	
	m_ImGuiLayer->AddWindow([&](bool& isVisible) {
		ImGui::SetNextWindowSize(ImVec2(300, 200));
		ImGui::Begin("Stats");
		ImGui::TextWrapped("  There were %d scopes! DeltaTime = %.3fms", Profiler::Get().ProfileDurations.size(), deltaTime);
		ImGui::TextWrapped("  FPS: %d", FPS);
		for (const Profiler::ProfilerDuration& d : Profiler::Get().ProfileDurations)
		{
			ImGui::Text((d.label + "     " + std::to_string(d.duration) + "ns").c_str());
		}
		ImGui::End();
	});

	while (IsRunning)
	{
		Profiler::ClearProfiles();

		ImGuiNextFrame();

		{
			calcFPS();
			YEASTEM_ScopedProfiler("APPLICATION: Frame");

			handleEvents(m_events);

			m_CurrentTime = SDL_GetTicks64();
			deltaTime = (m_CurrentTime - previous_time) / 1000.0f;
			deltaTime = SDL_min(deltaTime, 0.008f);
			previous_time = m_CurrentTime;

			if(CurrentScene.ShouldRefreshFrameBuffer) {
				CurrentScene.RecreateFrameBuffer(
					(uint32_t)CurrentScene.SceneSize.x,
					(uint32_t)CurrentScene.SceneSize.y
				);
				CurrentScene.ShouldRefreshFrameBuffer = false;
			}

			process_frame(deltaTime);
			accumulated_time += deltaTime;
		}

		m_ImGuiLayer->Update();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(m_Window);

		m_ImGuiLayer->BackupFrame();
	}

	m_ImGuiLayer->Destroy();

	SDL_GL_DeleteContext(m_Context);
	SDL_DestroyRenderer(s_Renderer);
}

YEASTEM_END	