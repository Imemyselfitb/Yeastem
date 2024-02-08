#include "yst_pch.h"

#include "Scene.h"

#include "Core/Application.h"

#include "Core/Lua/LuaVector2.h"

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

	LuaVector2::Init(this->m_LuaState);

	lua_newtable(this->m_LuaState);
	lua_setglobal(this->m_LuaState, "Yeastem");

	lua_newtable(this->m_LuaState);
	lua_pushstring(this->m_LuaState, "IsKeyDown");
	lua_pushcfunction(this->m_LuaState, [](lua_State* L) -> int {
		if (lua_isnumber(L, -1))
		{
			unsigned int k = lua_tonumber(L, -1);
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

void Scene::Update(float deltaTime, int windowWidth, int windowHeight)
{
	for (int i = 0; i < this->m_Shapes.size(); i++)
		this->UpdateScriptsFromObjects(i);

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
		shape.Translate({ shape.Position.x, (float)windowHeight - shape.Position.y });
		Vector2 Centre = shape.GetCentre();
		shape.Rotate(shape.Direction, Centre);
		shape.ScaleBy(shape.Scale, Centre);

		shape.UpdateVertices(windowWidth, windowHeight);
		shape.Pop();
	}

	this->CurrentTime += deltaTime * 1000.0f;
}

void Scene::Render()
{
	for (int i = 0; i < this->m_Shapes.size(); i++)
	{
		this->m_Shapes[i].shader->SetUniform1f("u_Time", this->CurrentTime / 1000.0f);
		this->m_Shapes[i].shader->SetUniform1i("u_ShapeID", i + 1);
		this->m_Renderer.Render(this->m_Shapes[i]);
	}
}

YEASTEM_END
