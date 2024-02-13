#include "yst_pch.h"

#include "Application.h"

#include "OpenGL/Include.h"

#include "LuaAPI/Lua.h"

YEASTEM_BEGIN

Application Application::s_Instance;
SDL_Renderer* Application::s_Renderer;
Scene Application::CurrentScene;
std::unique_ptr<Yeastem::ImGuiAPI> Application::s_ImGuiLayer;

bool Application::s_GLInitialized = false;
void Application::setupGlFlags() const
{
	if (s_GLInitialized) return;

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
	m_context = SDL_GL_CreateContext(m_window);

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	s_GLInitialized = true;
}

bool Application::s_IsRunning = false;
void Application::HandleEvents(SDL_Event& m_events) const
{
	while (SDL_PollEvent(&m_events))
	{
		ImGui_ImplSDL2_ProcessEvent(&m_events);
		if (m_events.type == SDL_QUIT)
		{
			s_IsRunning = false;
			return;
		}

		if (
			m_events.type == SDL_WINDOWEVENT && 
			m_events.window.event == SDL_WINDOWEVENT_CLOSE && 
			m_events.window.windowID == SDL_GetWindowID(this->m_window)
		) {
			s_IsRunning = false;
			return;
		}

		this->CurrentScene.ProcessEvent(m_events);
	}
}

void Application::SetupRender()
{
	Shape Paddle1;
	Shape Paddle2;
	Shape Ball;

	std::vector<Vertex> paddleVertices = {
		{ { -20.0f, -100.0f }, { 0.0f, 0.0f } },
		{ {  20.0f, -100.0f }, { 1.0f, 0.0f } },
		{ {  20.0f,  100.0f }, { 1.0f, 1.0f } },
		{ { -20.0f,  100.0f }, { 0.0f, 1.0f } }
	};

	std::vector<Vertex> ballVertices = {
		{ { -20.0f, -20.0f }, { 0.0f, 0.0f } },
		{ {  20.0f, -20.0f }, { 1.0f, 0.0f } },
		{ {  20.0f,  20.0f }, { 1.0f, 1.0f } },
		{ { -20.0f,  20.0f }, { 0.0f, 1.0f } }
	};

	std::vector<unsigned int> quadIndices = {
		0U, 1U, 2U,
		2U, 3U, 0U
	};

	Paddle1.Init(Vector2(0, 0));
	Paddle2.Init(Vector2(0, 0));
	Ball.Init(Vector2(0, 0));

	Paddle1.AssignVertices(paddleVertices);
	Paddle2.AssignVertices(paddleVertices);
	Ball.AssignVertices(ballVertices);
	Paddle1.AssignIndices(quadIndices);
	Paddle2.AssignIndices(quadIndices);
	Ball.AssignIndices(quadIndices);

	Paddle1.Position = Vector2(150.0f, 250.0f);
	Paddle2.Position = Vector2(850.0f, 250.0f);
	Ball.Position = Vector2(500.0f, 250.0f);

	VertexBufferLayout vbl;
	vbl.Push<float>(2);
	vbl.Push<float>(2);

	Paddle1.AddVertexBufferLayout(vbl);
	Paddle2.AddVertexBufferLayout(vbl);
	Ball.AddVertexBufferLayout(vbl);

	Paddle1.AssignShaderFromFiles("src/Shaders/Default.vert", "src/Shaders/Texture.frag");
	Paddle2.AssignShaderFromFiles("src/Shaders/Default.vert", "src/Shaders/Texture.frag");
	Ball.AssignShaderFromFiles("src/Shaders/Default.vert", "src/Shaders/Texture.frag");

	Paddle1.ReserveTextures(1);
	Paddle2.ReserveTextures(1);
	Ball.ReserveTextures(1);

	Paddle1.AddTexture("PongTest/Paddle.png");
	Paddle2.AddTexture("PongTest/Paddle.png");
	Ball.AddTexture("PongTest/Ball.png");

	this->CurrentScene.AddObject(Paddle1);
	this->CurrentScene.AddObject(Paddle2);
	this->CurrentScene.AddObject(Ball);
}

int seed = 0;
int pseudo_rand()
{
	seed = (seed - 1) * (seed ^ 0x02468) * 3;
	return seed;
}

void Application::process_frame(float deltaTime)
{
	SDL_GetWindowPosition(m_window, &m_WindowPosition.x, &m_WindowPosition.y);
	SDL_GetWindowSize(m_window, &m_WindowSize.w, &m_WindowSize.h);

	glViewport(0, 0, m_WindowSize.w, m_WindowSize.h);
	glClear(GL_COLOR_BUFFER_BIT);

	this->CurrentScene.Update(deltaTime, m_WindowSize.w, m_WindowSize.h);
	this->CurrentScene.Render();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_window);
}

void Application::initImGui()
{
	this->s_ImGuiLayer = std::make_unique<Yeastem::ImGuiAPI>(this->m_window, &this->m_context);
}

void Application::run()
{
	s_Renderer = SDL_CreateRenderer(m_window, -1, 0);

	this->s_ImGuiLayer->Init();

	s_IsRunning = true;

	// -- RUN SCRIPTS -- //
	this->CurrentScene.Lua_Init();
	this->SetupRender();
	this->CurrentScene.Lua_ExcecuteScript("PongTest/Panels.lua");
	this->CurrentScene.Lua_ExcecuteScript("PongTest/Game.lua");
	this->CurrentScene.Lua_ExcecuteScript("PongTest/RectCollisionModule.lua");

	SDL_Event m_events;
	unsigned long long previous_time = 0;
	float accumulated_time = 0;
	while (s_IsRunning)
	{
		this->HandleEvents(m_events);

		this->CurrentTime = SDL_GetTicks64();
		float deltaTime = (this->CurrentTime - previous_time) / 1000.0f;
		deltaTime = SDL_min(deltaTime, 0.008f);
		previous_time = this->CurrentTime;
			
		this->s_ImGuiLayer->Update();
		this->process_frame(deltaTime);

		this->s_ImGuiLayer->BackupFrame();
		accumulated_time += deltaTime;
	}

	this->s_ImGuiLayer->Destroy();

	SDL_GL_DeleteContext(this->m_context);
	SDL_DestroyRenderer(s_Renderer);
}

YEASTEM_END	