#include "yst_pch.h"

#include "Scene.h"

#include "Core/Application.h"

#include "Core/Lua/LuaVector2.h"
#include "Core/Lua/LuaPanel.h"

YEASTEM_BEGIN

void Scene::AddObject(Shape& shape)
{
	std::string str = "Object" + std::to_string(1 + (int)this->m_Shapes.size());
	LuaState& L = this->m_LuaState;

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

	this->m_Shapes.push_back(shape);
}

void Scene::Lua_Init()
{
	luaL_openlibs(this->m_LuaState);

	lua_newtable(this->m_LuaState);
	lua_setglobal(this->m_LuaState, "Yeastem");

	LuaImguiPanel::Init(this->m_LuaState);
	LuaVector2::Init(this->m_LuaState);

	lua_newtable(this->m_LuaState);
	lua_pushstring(this->m_LuaState, "IsKeyDown");
	lua_pushcfunction(this->m_LuaState, [](lua_State* L) -> int {
		if (lua_isnumber(L, -1))
		{
			unsigned int k = (unsigned int)lua_tonumber(L, -1);
			lua_pushboolean(L, Application::CurrentScene.m_EventHandler.IsKeyDown(k));
		} else if (lua_isstring(L, -1))
		{
			char key = std::tolower(lua_tostring(L, -1)[0]);
			lua_pushboolean(L, Application::CurrentScene.m_EventHandler.IsKeyDown(key));
		}
		return 1;
	});
	lua_settable(this->m_LuaState, -3);

	// More keycodes shall soon be added!
	this->AddKeyEnum("RightArrow", SDLK_RIGHT);
	this->AddKeyEnum("LeftArrow", SDLK_LEFT);
	this->AddKeyEnum("UpArrow", SDLK_UP);
	this->AddKeyEnum("DownArrow", SDLK_DOWN);

	lua_setglobal(this->m_LuaState, "Keys");

	lua_newtable(this->m_LuaState);
	lua_pushstring(this->m_LuaState, "width");
	lua_pushnumber(this->m_LuaState, 0);
	lua_settable(this->m_LuaState, -3);

	lua_pushstring(this->m_LuaState, "height");
	lua_pushnumber(this->m_LuaState, 0);
	lua_settable(this->m_LuaState, -3);
	lua_setglobal(this->m_LuaState, "Window");
}

void Scene::AddKeyEnum(const char* keyName, unsigned int keyCode)
{
	lua_pushstring(this->m_LuaState, keyName);
	lua_pushnumber(this->m_LuaState, keyCode);
	lua_settable(this->m_LuaState, -3);
}

void Scene::Lua_AttachCFunction(int(*nativeFunction)(lua_State*), const char* functionName)
{
	this->m_LuaState.addNativeFunction(nativeFunction, functionName);
}

void Scene::Lua_ExcecuteScript(const std::string& file)
{
	this->m_ScriptCount++;
	this->m_LuaState.executeScriptFromFile(file);
}

void Scene::UpdateObjectFromScripts(int shapeIdx) {
	Shape& shape = this->m_Shapes[shapeIdx];
	LuaState& L = this->m_LuaState;

	std::string str = "Object" + std::to_string(shapeIdx + 1);
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

void Scene::UpdateScriptsFromObjects(int shapeIdx) {
	Shape& shape = this->m_Shapes[shapeIdx];
	LuaState& L = this->m_LuaState;

	std::string str = "Object" + std::to_string(shapeIdx + 1);
	lua_getglobal(L, str.c_str());

	lua_pushnumber(L, shape.Position.x);
	lua_setfield(L, -2, "x");

	lua_pushnumber(L, shape.Position.y);
	lua_setfield(L, -2, "y");

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
	this->m_EventHandler.Dispatch(evt);
}

void Scene::Update(float deltaTime)
{
	for (int i = 0; i < this->m_Shapes.size(); i++)
		this->UpdateScriptsFromObjects(i);

	lua_getglobal(this->m_LuaState, "Window");
	lua_pushstring(this->m_LuaState, "width");
	lua_pushnumber(this->m_LuaState, this->SceneSize.x);
	lua_settable(this->m_LuaState, -3);

	lua_pushstring(this->m_LuaState, "height");
	lua_pushnumber(this->m_LuaState, this->SceneSize.y);
	lua_settable(this->m_LuaState, -3);
	lua_pop(this->m_LuaState, 1);

	lua_getglobal(this->m_LuaState, "Yeastem");
	for (int i = 0; i < this->m_ScriptCount; i++)
	{
		std::string funcName = (std::string)"Update_" + std::to_string(46290 + i);
		lua_getfield(this->m_LuaState, -1, funcName.c_str());
		if (lua_isfunction(this->m_LuaState, -1))
		{
			lua_pushnumber(this->m_LuaState, deltaTime);
			if (LUA_OK != lua_pcall(this->m_LuaState, 1, 0, 0))
			{
				const char* msg = lua_tostring(this->m_LuaState, -1);
				lua_writestringerror("%s: ", "Lua");
				lua_writestringerror("%s\n", msg);
				lua_pop(this->m_LuaState, 1);
			}
		}
		else lua_pop(this->m_LuaState, 1);
	}
	lua_pop(this->m_LuaState, 1);

	for (int i = 0; i < this->m_Shapes.size(); i++)
	{
		Shape& shape = this->m_Shapes[i];

		UpdateObjectFromScripts(i);

		shape.Push();
		shape.Translate({ shape.Position.x, this->SceneSize.y - shape.Position.y });
		Vector2 Centre = shape.GetCentre();
		shape.Rotate(shape.Direction, Centre);
		shape.ScaleBy(shape.Scale, Centre);

		shape.UpdateVertices((int)this->SceneSize.x, (int)this->SceneSize.y);
		shape.Pop();
	}

	LuaImguiPanel::ShowAll(this->m_LuaState);

	this->CurrentTime += uint64_t(deltaTime * 1000.0f);
}

void Scene::RecreateFrameBuffer(unsigned int width, unsigned int height)
{
	if (!this->m_FrameBuffer)
		return this->CreateFrameBuffer(width, height);

	this->m_FrameBuffer->Destroy();
	this->m_FrameBuffer->Create(width, height);
}

void Scene::CreateFrameBuffer(unsigned int width, unsigned int height)
{
	this->m_FrameBuffer = std::make_unique<FrameBuffer>(width, height);
}

void Scene::Render()
{
	if (this->m_FrameBuffer)
	{
		this->m_FrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	for (int i = 0; i < this->m_Shapes.size(); i++)
	{
		this->m_Shapes[i].shader->SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
		this->m_Shapes[i].shader->SetUniform1i("u_ShapeID", i + 1);
		this->m_Renderer.Render(this->m_Shapes[i]);
	}

	if (this->m_FrameBuffer)
		this->m_FrameBuffer->Unbind();
}


YEASTEM_END
