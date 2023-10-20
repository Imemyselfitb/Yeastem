#include "yst_pch.h"

#include "Application.h"

#include "OpenGL/Include.h"

YEASTEM_BEGIN

Application Application::s_Instance;

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

void Application::SetupRender()
{
	unsigned int vertDim = 2; // Num of Dimmensions (E.g 2D, 3D) of the vertex
	float vertices[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};

	unsigned int _indexCount = 6; // Length of indices Buffer. 
	unsigned int _length = _indexCount * vertDim;
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	this->m_Renderer.AssignVertexBuffer(vertices, 4 * 2 * sizeof(float));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, vertDim, GL_FLOAT, GL_FALSE, vertDim * sizeof(float), 0);

	this->m_Renderer.AssignIndexBuffer(indices, _indexCount);


	this->m_Renderer.AssignShaderFromFiles(
		"src/Shaders/Default.vert",
		"src/Shaders/Default.frag"
	);
}

static float r = 0.0f;
static float inc = 0.01f;
void Application::render_frame()
{
	SDL_GetWindowPosition(m_window, &m_WindowPosition.x, &m_WindowPosition.y);
	SDL_GetWindowSize(m_window, &m_WindowSize.w, &m_WindowSize.h);

	glViewport(0, 0, m_WindowSize.w, m_WindowSize.h);

	this->m_Renderer.BindIndexBuffer();
	this->m_Renderer.BindVertexBuffer();
	this->m_Renderer.BindShader();

	int location = glGetUniformLocation(this->m_Renderer.getShader(), "u_Colour");
	if (location != -1) glUniform4f(location, r, 0.7f, 0.9f, 1.0f);

	r += inc;
	if (r > 1.0f) inc *= -1;
	if (r < 0.0f) inc *= -1;

	/* RENDERING */
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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
	s_IsRunning = true;
	
	this->SetupRender();
	SDL_Renderer* m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	this->m_ImGuiLayer->Init();

	SDL_Event m_events;
	while (s_IsRunning)
	{
		this->HandleEvents(m_events);

		this->m_ImGuiLayer->Update();
		this->render_frame();

		this->m_ImGuiLayer->BackupFrame();
	}

	this->m_ImGuiLayer->Destroy();

	SDL_GL_DeleteContext(this->m_context);
	SDL_DestroyRenderer(m_renderer);
}

YEASTEM_END