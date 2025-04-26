#include "pch.h"

void SetOpenGlFlags() {
	// Set Version of Glad (4.6)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// Use core Profile, and Double Buffer
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Use a Depth size of 24
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}


static GLuint VBO;
static GLuint IBO;
using VertexBufferType = float;
using IndexBufferType = unsigned int;

static std::vector<VertexBufferType> vertices;
static std::vector<IndexBufferType> indices;

void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cout << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
		" type = " << type <<
		", severity = " << severity <<
		", message = " << message << std::endl;
}

void addRenderObject(VertexBufferType verts[], IndexBufferType inds[])
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Add indices to the indices vector
	for (int i = 0; i < 6; i++)
	{
		indices.push_back(inds[i] + (IndexBufferType)(vertices.size() / 2));
	}

	// Add vertices to the vertices vector
	for (int i = 0; i < 8; i++)
	{
		vertices.push_back(verts[i]);
	}

	return;
}

void SetupOpenGlScene()
{
	VertexBufferType vertices1[] = {
		-0.75f, -0.25f,
		-0.75f,  0.25f,
		-0.25f,  0.25f,
		-0.25f, -0.25f
	};

	VertexBufferType vertices2[] = {
		0.25f, -0.25f,
		0.25f,  0.25f,
		0.75f,  0.25f,
		0.75f, -0.25f
	};

	IndexBufferType indices1[] = {
		0, 1, 2, 
		2, 3, 0
	};

	IndexBufferType indices2[] = {
		0, 1, 2, 
		2, 3, 0
	};

	addRenderObject(vertices1, indices1);
	addRenderObject(vertices2, indices2);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexBufferType), vertices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(VertexBufferType), 0);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(IndexBufferType), indices.data(), GL_DYNAMIC_DRAW);
}

void RenderOpenGlScene()
{
#define USING_IBO 1
#if USING_IBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size() * sizeof(IndexBufferType), GL_UNSIGNED_INT, nullptr);
#else
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

int main(int argc, char* argv[])
{
	SetOpenGlFlags();

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "SDL could not be initialized: " << SDL_GetError();
	}

	int flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	SDL_Window* Window = SDL_CreateWindow("Run Time!", 600, 600, flags);

	SDL_GLContext Context = SDL_GL_CreateContext(Window);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	SDL_GL_MakeCurrent(Window, Context);

	SDL_Event Events;
	bool Running = true;

	std::cout << glGetString(GL_VERSION) << std::endl;

	SetupOpenGlScene();

	uint64_t frame = 0;
	while (Running)
	{
		frame++;
		while (SDL_PollEvent(&Events))
		{
			if (Events.type == SDL_EVENT_QUIT)
			{
				Running = false;
				break;
			}
		}
		if (!Running) break;

		int Width, Height;
		SDL_GetWindowSize(Window, &Width, &Height);
		glViewport(0, 0, Width, Height);
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		RenderOpenGlScene();

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		}

		const char* error = SDL_GetError();
		if (*error) {
			std::cerr << "SDL Error: " << error << std::endl;
			SDL_ClearError();  // Clear the error message after retrieving it
		}

		SDL_GL_SwapWindow(Window);
	}

	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);

	SDL_GL_DestroyContext(Context);
	SDL_DestroyWindow(Window); 
	SDL_Quit();

	return 0;
}
