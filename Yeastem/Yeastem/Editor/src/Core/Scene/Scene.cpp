#include "yst_pch.h"

#include "Scene.h"

YEASTEM_BEGIN

void Scene::AddObject(Shape& shape)
{
	std::string str = "Object" + std::to_string(1 + (int)this->m_Shapes.size());
	LuaState& L = this->m_LuaState;
	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, shape.Position.x);
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, shape.Position.y);
	lua_settable(L, -3);
	lua_pushstring(L, "dir");
	lua_pushnumber(L, shape.Direction);
	lua_settable(L, -3);
	lua_pushstring(L, "scale");
	lua_pushnumber(L, shape.Scale);
	lua_settable(L, -3);
	lua_setglobal(L, str.c_str());

	this->m_Shapes.push_back(shape);
}

void Scene::Lua_Init()
{
	luaL_openlibs(this->m_LuaState);
}

void Scene::Lua_AttachCFunction(int(*nativeFunction)(lua_State*), const char* functionName)
{
	this->m_LuaState.addNativeFunction(nativeFunction, functionName);
}

void Scene::Lua_ExcecuteScript(const std::string& file)
{
	this->m_LuaState.executeScriptFromFile(file);
}

void Scene::UpdateScripts(int shapeIdx, float deltaTime) {
	Shape& shape = this->m_Shapes[shapeIdx];
	LuaState& L = this->m_LuaState;

	std::string str = "Object" + std::to_string(shapeIdx + 1);
	lua_getglobal(L, str.c_str());

	lua_getfield(L, -1, "x");
	shape.Position.x = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	shape.Position.y = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "dir");
	shape.Direction = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "scale");
	shape.Scale = (float)lua_tonumber(L, -1);
	lua_pop(L, 2);
}

void Scene::Update(float deltaTime, int windowWidth, int windowHeight)
{
	lua_getglobal(this->m_LuaState, "Update_46290");
	if (lua_isfunction(this->m_LuaState, -1))
	{
		lua_pushnumber(this->m_LuaState, deltaTime);
		lua_pcall(this->m_LuaState, 1, 0, 0);
	}
	else lua_pop(this->m_LuaState, 1);

	lua_getglobal(this->m_LuaState, "Update_46291");
	if (lua_isfunction(this->m_LuaState, -1))
	{
		lua_pushnumber(this->m_LuaState, deltaTime);
		lua_pcall(this->m_LuaState, 1, 0, 0);
	}
	else lua_pop(this->m_LuaState, 1);

	for (int i = 0; i < this->m_Shapes.size(); i++)
	{
		Shape& shape = this->m_Shapes[i];

		UpdateScripts(i, deltaTime);

		shape.Push();
		shape.Translate({ shape.Position.x, (float)windowHeight - shape.Position.y });
		Vector2 Centre = shape.GetCentre();
		shape.Rotate(shape.Direction, Centre);
		shape.ScaleBy(shape.Scale, Centre);

		shape.UpdateVertices(windowWidth, windowHeight);
		shape.Pop();
	}
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
