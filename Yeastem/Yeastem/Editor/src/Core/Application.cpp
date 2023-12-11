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

Renderable Object1;
Renderable Object2;
void Application::SetupRender()
{
	unsigned int vertDim = 4; // Attributes Per Vertex
	unsigned int vertexCount = 4; // Total Count of Vertices
	
	// vert x, vert y, texture x, texture y
	float vertices1[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		-0.02f, -0.5f, 1.0f, 0.0f,
		-0.02f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
	};

	float vertices2[] = {
		 0.02f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		 0.02f,  0.5f, 0.0f, 1.0f
	};

	unsigned int _indexCount = 6; // Length of indices Buffer. 
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	Object1.AssignVertexBuffer(vertices1, vertexCount * vertDim * sizeof(float));
	Object2.AssignVertexBuffer(vertices2, vertexCount * vertDim * sizeof(float));
	Object1.AssignIndexBuffer(indices, _indexCount);
	Object2.AssignIndexBuffer(indices, _indexCount);

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

	//CreateTexture("test/PNG_Texture.png", tex0);
	//CreateTexture("test/MyTexture.png", tex0);
	Object1.ReserveTextures(2);
	Object2.ReserveTextures(2);

	Object1.AddTexture("test/PNG_Texture.png");
	Object1.AddTexture("test/MyTexture.png");

	Object2.AddTexture("test/OnlyFrame.png");
	Object2.AddTexture("test/Scenery.png");
}


void Application::render_frame()
{
	SDL_GetWindowPosition(m_window, &m_WindowPosition.x, &m_WindowPosition.y);
	SDL_GetWindowSize(m_window, &m_WindowSize.w, &m_WindowSize.h);

	glViewport(0, 0, m_WindowSize.w, m_WindowSize.h);
	glClear(GL_COLOR_BUFFER_BIT);

	Object1.shader.SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
	Object2.shader.SetUniform1f("u_Time", this->CurrentTime / 1000.0f);

	Object1.shader.SetUniform1i("u_ShapeID", 1);
	Object2.shader.SetUniform1i("u_ShapeID", 2);

	this->m_Renderer.Render(Object1);
	this->m_Renderer.Render(Object2);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_window);
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
	while (s_IsRunning)
	{
		this->HandleEvents(m_events);
		this->CurrentTime = SDL_GetTicks64();

		this->m_ImGuiLayer->Update();
		this->render_frame();

		this->m_ImGuiLayer->BackupFrame();
	}

	this->m_ImGuiLayer->Destroy();

	SDL_GL_DeleteContext(this->m_context);
	SDL_DestroyRenderer(s_Renderer);
}

YEASTEM_END