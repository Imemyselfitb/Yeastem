#include "yst_pch.h"

#include "Scene.h"

#include "Core/Application.h"

#include "Core/Lua/LuaVector2.h"
#include "Core/Lua/LuaPanel.h"

YEASTEM_BEGIN

void Scene::Init(ResourceManager& resourceManager)
{
	m_Renderer.Init(resourceManager);
}

ObjectID Scene::CreateObject()
{
	ObjectID objID = static_cast<ObjectID>(m_Shapes.size());
	//ObjectID objID;
	//do objID = rand();
	//while (m_Shapes.contains(objID));
	CreateObjectInScript(m_Shapes[objID], "Object" + std::to_string(1 + (int)m_Shapes.size()));
	return objID;
}

Shape<QuadVertex>& Scene::GetObjectWithID(ObjectID id)
{
	return m_Shapes.at(id);
}

void Scene::CreateObjectInScript(Shape<QuadVertex>& shape, const std::string& str)
{
	LuaState& L = m_LuaState;

	lua_newtable(L);
	lua_pushstring(L, "Position");
	LuaVector2::InitVector(shape.Position, L);
	lua_settable(L, -3);

	lua_pushstring(L, "dir");
	lua_pushnumber(L, shape.Direction);
	lua_settable(L, -3);

	lua_pushstring(L, "scale");
	lua_pushnumber(L, shape.Scale);
	lua_settable(L, -3);

	lua_pushstring(L, "width");
	lua_pushnumber(L, shape.Size.x);
	lua_settable(L, -3);

	lua_pushstring(L, "height");
	lua_pushnumber(L, shape.Size.y);
	lua_settable(L, -3);
	lua_setglobal(L, str.c_str());
}

void Scene::Lua_Init()
{
	m_ScriptCount = 0;

	luaL_openlibs(m_LuaState);

	lua_newtable(m_LuaState);
	lua_setglobal(m_LuaState, "Yeastem");

	LuaImguiPanel::Init(m_LuaState);
	LuaVector2::Init(m_LuaState);

	lua_newtable(m_LuaState);
	lua_pushstring(m_LuaState, "IsKeyDown");
	lua_pushcfunction(m_LuaState, [](lua_State* L) -> int {
		if (lua_isnumber(L, -1))
		{
			uint32_t k = (uint32_t)lua_tonumber(L, -1);
			lua_pushboolean(L, Application::Get().CurrentScene.m_EventHandler.IsKeyDown(k));
		}
		else if (lua_isstring(L, -1))
		{
			char key = std::tolower(lua_tostring(L, -1)[0]);
			lua_pushboolean(L, Application::Get().CurrentScene.m_EventHandler.IsKeyDown(key));
		}
		return 1;
	});
	lua_settable(m_LuaState, -3);

	// More keycodes shall soon be added!
	AddKeyEnum("RightArrow", SDLK_RIGHT);
	AddKeyEnum("LeftArrow", SDLK_LEFT);
	AddKeyEnum("UpArrow", SDLK_UP);
	AddKeyEnum("DownArrow", SDLK_DOWN);

	lua_setglobal(m_LuaState, "Keys");

	lua_newtable(m_LuaState);
	lua_pushstring(m_LuaState, "width");
	lua_pushnumber(m_LuaState, 0);
	lua_settable(m_LuaState, -3);

	lua_pushstring(m_LuaState, "height");
	lua_pushnumber(m_LuaState, 0);
	lua_settable(m_LuaState, -3);
	lua_setglobal(m_LuaState, "Window");
}

void Scene::AddKeyEnum(const char* keyName, uint32_t keyCode)
{
	lua_pushstring(m_LuaState, keyName);
	lua_pushnumber(m_LuaState, keyCode);
	lua_settable(m_LuaState, -3);
}

void Scene::Lua_AttachCFunction(int(*nativeFunction)(lua_State*), const char* functionName)
{
	m_LuaState.addNativeFunction(nativeFunction, functionName);
}

void Scene::Lua_ExcecuteScript(const std::string& file)
{
	if (!FileIO::checkIfExists(file)) return;

	m_ScriptCount += 1;
	m_LuaState.executeScriptFromFile(file);
}

void Scene::Lua_AttachScript(const std::string& file)
{
	m_Scripts.push_back(file);
}

void Scene::Lua_RebuildState()
{
	m_LuaState.CreateNewState();

	Lua_Init();

	for (const std::string& file : m_Scripts)
		Lua_ExcecuteScript(file);

	for (int i = 0; i < m_Shapes.size(); i++)
		CreateObjectInScript(m_Shapes[i], "Object" + std::to_string(i + 1));
}

void Scene::UpdateObjectFromScripts(ObjectID id) {
	Shape<QuadVertex>& shape = GetObjectWithID(id);
	LuaState& L = m_LuaState;

	std::string str = "Object" + std::to_string(id + 1);
	lua_getglobal(L, str.c_str());

	lua_getfield(L, -1, "Position");
	LuaVector2::UpdateVector(shape.Position, L);
	lua_pop(L, 1);

	lua_getfield(L, -1, "dir");
	shape.Direction = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "scale");
	shape.Scale = (float)lua_tonumber(L, -1);
	lua_pop(L, 2);
}

void Scene::UpdateScriptsFromObjects(ObjectID id) {
	Shape<QuadVertex>& shape = GetObjectWithID(id);
	LuaState& L = m_LuaState;

	std::string str = "Object" + std::to_string(id + 1);
	lua_getglobal(L, str.c_str());
	if (lua_isnil(L, -1))
	{
		YEASTEM_ERROR("Object `" << str << "` not Created to Lua!");
		lua_pop(L, 1);
		return;
	}

	{
		lua_getfield(L, -1, "Position");

		lua_pushnumber(L, shape.Position.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, shape.Position.y);
		lua_setfield(L, -2, "y");

		lua_pop(L, 1);
	}

	lua_pushnumber(L, shape.Direction);
	lua_setfield(L, -2, "dir");

	lua_pushnumber(L, shape.Scale);
	lua_setfield(L, -2, "scale");

	lua_pushnumber(L, shape.Size.x);
	lua_setfield(L, -2, "width");

	lua_pushnumber(L, shape.Size.y);
	lua_setfield(L, -2, "height");

	lua_pop(L, 1);
}

void Scene::ProcessEvent(SDL_Event& evt)
{
	if (evt.type == SDL_KEYDOWN && !evt.key.repeat)
	{
		if (evt.key.keysym.sym == SDLK_h)
		{
			std::cout << "PLAY/PAUSE [H]\n";
			IsRunning = !IsRunning;
			IsReset = false;
		}
		else if (evt.key.keysym.sym == SDLK_j)
		{
			std::cout << "RESET [J]\n";
			IsRunning = false;
			IsReset = true;
		}
	}

	m_EventHandler.Dispatch(evt);
}

void Scene::Update(float deltaTime)
{
	if (SceneSize.x == 0 || SceneSize.y == 0)
		return;

	if (IsRunning)
	{
		if (m_ScriptCount != m_Scripts.size())
			Lua_RebuildState();

		for (auto& [shapeID, shape] : m_Shapes)
			UpdateScriptsFromObjects(shapeID);

		lua_getglobal(m_LuaState, "Window");
		lua_pushstring(m_LuaState, "width");
		lua_pushnumber(m_LuaState, SceneSize.x);
		lua_settable(m_LuaState, -3);

		lua_pushstring(m_LuaState, "height");
		lua_pushnumber(m_LuaState, SceneSize.y);
		lua_settable(m_LuaState, -3);
		lua_pop(m_LuaState, 1);

		lua_getglobal(m_LuaState, "Yeastem");
		for (int i = 0; i < m_ScriptCount; i++)
		{
			std::string funcName = (std::string)"Update_" + std::to_string(46290 + i);
			lua_getfield(m_LuaState, -1, funcName.c_str());
			if (lua_isfunction(m_LuaState, -1))
			{
				lua_pushnumber(m_LuaState, deltaTime);
				if (LUA_OK != lua_pcall(m_LuaState, 1, 0, 0))
				{
					const char* msg = lua_tostring(m_LuaState, -1);
					lua_writestringerror("%s: ", "Lua");
					lua_writestringerror("%s\n", msg);
					lua_pop(m_LuaState, 1);
				}
			}
			else lua_pop(m_LuaState, 1);
		}
		lua_pop(m_LuaState, 1);
	}
	else if (IsReset)
		m_ScriptCount = -1;

	for (auto& [shapeID, shape] : m_Shapes)
	{
		if (IsRunning)
			UpdateObjectFromScripts(shapeID);

		if (IsReset)
		{
			shape.Position = shape.DefaultPosition;
			shape.Direction = shape.DefaultDirection;
			shape.Scale = shape.DefaultScale;
		}
	}

	if (IsRunning)
	{
		LuaImguiPanel::ShowAll(m_LuaState);
		CurrentTime += uint64_t(deltaTime * 1000.0f);
	}
}

void Scene::RecreateFrameBuffer(uint32_t width, uint32_t height)
{
	if (!m_FrameBuffer)
		return CreateFrameBuffer(width, height);

	m_FrameBuffer->Destroy();
	m_FrameBuffer->Create(width, height);
}

void Scene::CreateFrameBuffer(uint32_t width, uint32_t height)
{
	m_FrameBuffer = std::make_unique<FrameBuffer>(width, height);
}

void Scene::Render(ResourceManager& resourceManager)
{
	if (SceneSize.x == 0 || SceneSize.y == 0)
		return;

	if (m_FrameBuffer)
	{
		m_FrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	m_Renderer.BeginScene(resourceManager);

	for (auto& [shapeID, shape] : m_Shapes)
		m_Renderer.Submit(shape, resourceManager, SceneSize.x, SceneSize.y);

	m_Renderer.EndScene(resourceManager);

	if (m_FrameBuffer)
		m_FrameBuffer->Unbind();
}


YEASTEM_END
