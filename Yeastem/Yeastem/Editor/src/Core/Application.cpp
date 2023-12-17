#include "yst_pch.h"

#include "Application.h"

#include "OpenGL/Include.h"

YEASTEM_BEGIN

Application Application::s_Instance;
SDL_Renderer* Application::s_Renderer;

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
	}
}

Shape Object1;
Shape Object2;
void Application::SetupRender()
{
	std::vector<Vertex> squareVerts = {
		{ { -125.0f, -125.0f }, { 0.0f, 0.0f } },
		{ {  125.0f, -125.0f }, { 1.0f, 0.0f } },
		{ {  125.0f,  125.0f }, { 1.0f, 1.0f } },
		{ { -125.0f,  125.0f }, { 0.0f, 1.0f } }
	};

	unsigned int _indexCount = 6; // Length of indices Buffer. 
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Object1.Init();
	Object2.Init();

	Object1.AssignVertices(squareVerts);
	Object2.AssignVertices(squareVerts);
	Object1.AssignIndexBuffer(indices, _indexCount);
	Object2.AssignIndexBuffer(indices, _indexCount);

	for (PointMass& pt : Object1.GetPoints())
		pt.Position += Vector2(250.0f, 200.0f);
	for (PointMass& pt : Object2.GetPoints())
		pt.Position += Vector2(750.0f, 250.0f);

	VertexBufferLayout vbl;
	vbl.Push<float>(2);
	vbl.Push<float>(2);
	Object1.AddVertexBufferLayout(vbl);
	Object2.AddVertexBufferLayout(vbl);

	Object1.AssignShaderFromFiles(
		"src/Shaders/Default.vert",
		"src/Shaders/Texture.frag"
	);
	Object2.AssignShaderFromFiles(
		"src/Shaders/Default.vert",
		"src/Shaders/Texture.frag"
	);

	Object1.ReserveTextures(2);
	Object2.ReserveTextures(2);

	Object1.AddTexture("test/PNG_Texture.png");
	Object1.AddTexture("test/MyTexture.png");

	Object2.AddTexture("test/OnlyFrame.png");
	Object2.AddTexture("test/Scenery.png");

	const Renderable& objRend1 = Object1;
	const Renderable& objRend2 = Object1;
}

int seed = 0;
int pseudo_rand()
{
	seed = (seed - 1) * (seed ^ 0x02468) * 3;
	return seed;
}

float deltaTime;
float accTime;

void Application::render_frame()
{
	SDL_GetWindowPosition(m_window, &m_WindowPosition.x, &m_WindowPosition.y);
	SDL_GetWindowSize(m_window, &m_WindowSize.w, &m_WindowSize.h);

	int Mouse_X, Mouse_Y;
	SDL_GetMouseState(&Mouse_X, &Mouse_Y);

	{
		for (PointMass& pt : Object1.GetPoints())
		{
			pt.Position.x += cos(accTime * 5) * 500.0f * deltaTime;
			pt.Position.y += sin(accTime * 5) * 300.0f * deltaTime;
		}
	}

	{
		Vector2 centre = Object2.GetCentre();
		for (PointMass& pt : Object2.GetPoints())
			pt.Position.Rotate(deltaTime, centre);
	}

	Object1.UpdateVertices(m_WindowSize.w, m_WindowSize.h);
	Object2.UpdateVertices(m_WindowSize.w, m_WindowSize.h);

	glViewport(0, 0, m_WindowSize.w, m_WindowSize.h);
	glClear(GL_COLOR_BUFFER_BIT);

	Object1.shader->SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
	Object1.shader->SetUniform1i("u_ShapeID", 1);
	this->m_Renderer.Render(Object1);

	Object2.shader->SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
	Object2.shader->SetUniform1i("u_ShapeID", 2);
	this->m_Renderer.Render(Object2);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_window);

	accTime += deltaTime;
}

void Application::initImGui()
{
	this->m_ImGuiLayer = std::make_unique<Yeastem::ImGuiAPI>(this->m_window, &this->m_context);
}

void Application::run()
{
	this->SetupRender();
	s_Renderer = SDL_CreateRenderer(m_window, -1, 0);

	this->m_ImGuiLayer->Init();

	s_IsRunning = true;

	SDL_Event m_events;
	unsigned long long previous_time = 0;
	while (s_IsRunning)
	{
		this->HandleEvents(m_events);
		this->CurrentTime = SDL_GetTicks64();
		deltaTime = (this->CurrentTime - previous_time) / 1000.0f;
		deltaTime = SDL_min(deltaTime, 0.004f);
		previous_time = this->CurrentTime;

		this->m_ImGuiLayer->Update();
		this->render_frame();

		this->m_ImGuiLayer->BackupFrame();
	}

	this->m_ImGuiLayer->Destroy();

	SDL_GL_DeleteContext(this->m_context);
	SDL_DestroyRenderer(s_Renderer);
}

YEASTEM_END	