#include "yst_pch.h"

#include "Application.h"

#include "OpenGL/Include.h"
#include "Scene/Scene.h"

#include "LuaAPI/Lua.h"

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

Scene CurrentScene;

void Application::SetupRender()
{
	Shape Object1;
	Shape Object2;

	std::vector<Vertex> squareVertices = {
		{ { -125.0f, -125.0f }, { 0.0f, 0.0f } },
		{ {  125.0f, -125.0f }, { 1.0f, 0.0f } },
		{ {  125.0f,  125.0f }, { 1.0f, 1.0f } },
		{ { -125.0f,  125.0f }, { 0.0f, 1.0f } }
	};

	std::vector<unsigned int> squareIndices = {
		0U, 1U, 2U,
		2U, 3U, 0U
	};

	Object1.Init(Vector2(0, 0));
	Object2.Init(Vector2(0, 0));

	Object1.AssignVertices(squareVertices);
	Object2.AssignVertices(squareVertices);
	Object1.AssignIndices(squareIndices);
	Object2.AssignIndices(squareIndices);

	Object1.Position = Vector2(250.0f, 200.0f);
	Object2.Position = Vector2(750.0f, 200.0f);

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

	CurrentScene.AddObject(Object1);
	CurrentScene.AddObject(Object2);
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


	int Mouse_X, Mouse_Y;
	SDL_GetMouseState(&Mouse_X, &Mouse_Y);
	Vector2 Mouse(Mouse_X, Mouse_Y);

	/*
	{
		float t_x = cos(accTime * 5) * 500.0f * deltaTime;
		float t_y = sin(accTime * 5) * 300.0f * deltaTime;
		Object1.Position += Vector2(t_x, t_y);
	}

	{
		Object2.Position += (Mouse - Object2.Position) * 5.0f * deltaTime;
		Object2.Direction += deltaTime;
		Object2.Scale += sin(accTime * 5) * 1.0f * deltaTime;
	}

	Object1.Push();
	Object2.Push();
	
	Object1.Translate({ Object1.Position.x, (float)m_WindowSize.h - Object1.Position.y });
	Object2.Translate({ Object2.Position.x, (float)m_WindowSize.h - Object2.Position.y });

	Object1.Rotate(Object1.Direction, Object1.GetCentre());
	Object2.Rotate(Object2.Direction, Object2.GetCentre());

	Object1.ScaleBy(Object1.Scale, Object1.GetCentre());
	Object2.ScaleBy(Object2.Scale, Object2.GetCentre());

	Object1.UpdateVertices(m_WindowSize.w, m_WindowSize.h);
	Object2.UpdateVertices(m_WindowSize.w, m_WindowSize.h);

	Object1.Pop();
	Object2.Pop();

	Object1.shader->SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
	Object1.shader->SetUniform1i("u_ShapeID", 1);
	this->m_Renderer.Render(Object1);

	Object2.shader->SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
	Object2.shader->SetUniform1i("u_ShapeID", 2);
	this->m_Renderer.Render(Object2);
	*/

	CurrentScene.Update(deltaTime, m_WindowSize.w, m_WindowSize.h);
	CurrentScene.Render();


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
	s_Renderer = SDL_CreateRenderer(m_window, -1, 0);

	this->m_ImGuiLayer->Init();

	s_IsRunning = true;

	// -- RUN SCRIPTS -- //
	auto print = [](lua_State* L) -> int {
		int nargs = lua_gettop(L);

		std::cout << "Yeastem: Script: ";
		for (int i = 1; i <= nargs; i++) {
			if (lua_isstring(L, i))
				std:: cout << lua_tostring(L, i);
			else if (lua_isnumber(L, i))
				std::cout << lua_tonumber(L, i);
			else if (lua_isboolean(L, i))
				std::cout << (lua_toboolean(L, i) ? "True" : "False");
			else
				std::cout << lua_typename(L, i);
			
			if(i < nargs) 
				std::cout << " ";
		}

		std::cout << std::endl;

		return 1;
	};

	CurrentScene.Lua_Init();
	CurrentScene.Lua_AttachCFunction(print, "print");
	this->SetupRender();

	//CurrentScene.Lua_ExcecuteScript("test/Test.lua");
	CurrentScene.Lua_ExcecuteScript("test/Test2a.lua");
	CurrentScene.Lua_ExcecuteScript("test/Test2b.lua");

	SDL_Event m_events;
	unsigned long long previous_time = 0;
	float accumulated_time = 0;
	while (s_IsRunning)
	{
		this->HandleEvents(m_events);
		this->CurrentTime = SDL_GetTicks64();
		CurrentScene.CurrentTime = this->CurrentTime;

		float deltaTime = (this->CurrentTime - previous_time) / 1000.0f;
		deltaTime = SDL_min(deltaTime, 0.004f);
		previous_time = this->CurrentTime;

		this->m_ImGuiLayer->Update();
		
		//lua_getglobal(L, "Update");
		//if (lua_isfunction(L, -1))
		//{
		//	lua_pushnumber(L, deltaTime);
		//	lua_pcall(L, /*ParamCount*/1, /*ReturnCount*/0, /*ErrorHandler*/0);
		//}

		//lua_getglobal(L, "Object1");
		//lua_getfield(L, -1, "x");
		//Object1.Position.x = (float)lua_tonumber(L, -1);
		//lua_getglobal(L, "Object1");
		//lua_getfield(L, -1, "y");
		//Object1.Position.y = (float)lua_tonumber(L, -1);
		//lua_getglobal(L, "Object1");
		//lua_getfield(L, -1, "dir");
		//Object1.Direction = (float)lua_tonumber(L, -1);
		//lua_getglobal(L, "Object1");
		//lua_getfield(L, -1, "scale");
		//Object1.Scale = (float)lua_tonumber(L, -1);
		// 
		//lua_getglobal(L, "Object2");
		//lua_getfield(L, -1, "x");
		//Object2.Position.x = (float)lua_tonumber(L, -1);
		//lua_getglobal(L, "Object2");
		//lua_getfield(L, -1, "y");
		//Object2.Position.y = (float)lua_tonumber(L, -1);
		//lua_getglobal(L, "Object2");
		//lua_getfield(L, -1, "dir");
		//Object2.Direction = (float)lua_tonumber(L, -1);
		//lua_getglobal(L, "Object2");
		//lua_getfield(L, -1, "scale");
		//Object2.Scale = (float)lua_tonumber(L, -1);

		this->process_frame(deltaTime);

		this->m_ImGuiLayer->BackupFrame();
		accumulated_time += deltaTime;
	}

	this->m_ImGuiLayer->Destroy();

	SDL_GL_DeleteContext(this->m_context);
	SDL_DestroyRenderer(s_Renderer);
}

YEASTEM_END	