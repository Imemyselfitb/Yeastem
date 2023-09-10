#include "glad.c"

#include "Application.h"
#include "Events/Window.h"

#include "Shader.h"

#include <iostream>

YEASTEM_BEGIN

Application Application::s_Instance;

bool Application::s_GLInitialized = false;
void Application::setupGlFlags()
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
void Application::HandleEvents(SDL_Event& m_events)
{
	while (SDL_PollEvent(&m_events))
	{
		if (m_events.type == SDL_QUIT)
		{
			s_IsRunning = false;
			return;
		}
	}
}

void WindowEvent::Repaint()
{
	// Application::Render();
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

	unsigned int bufferID;
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, _length * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, vertDim, GL_FLOAT, GL_FALSE, vertDim * sizeof(float), 0);

	unsigned int indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	unsigned int shader = CreateShaderFromFiles(
		"src/Shaders/Default.vert",
		"src/Shaders/Default.frag"
	);
	glUseProgram(shader);
}

void Application::render_frame()
{
	SDL_GetWindowPosition(m_window, &m_WindowPosition.x, &m_WindowPosition.y);
	SDL_GetWindowSize(m_window, &m_WindowSize.w, &m_WindowSize.h);

	glViewport(0, 0, m_WindowSize.w, m_WindowSize.h);

	/* RENDERING */
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	SDL_GL_SwapWindow(m_window);
}

void Application::run()
{
	s_IsRunning = true;
	
	this->SetupRender();
	SDL_Renderer* m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	SDL_Event m_events;
	while (s_IsRunning)
	{
		this->HandleEvents(m_events);
		this->render_frame();
	}

	SDL_DestroyRenderer(m_renderer);
}

YEASTEM_END