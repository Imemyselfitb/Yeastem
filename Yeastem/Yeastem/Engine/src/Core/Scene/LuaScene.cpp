#include "yst_pch.h"

#include "LuaScene.h"

#include "Core/Application.h"
#include "Core/Scene/Scene.h"

#include "Core/Lua/LuaPanel.h"
#include "Core/Lua/LuaVector2.h"

#include "FileIO/FileIO.h"

#include "Components.h"

YEASTEM_BEGIN

void LuaScene::Init()
{
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
	addKeyEnum("RightArrow", SDLK_RIGHT);
	addKeyEnum("LeftArrow", SDLK_LEFT);
	addKeyEnum("UpArrow", SDLK_UP);
	addKeyEnum("DownArrow", SDLK_DOWN);
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

int LuaScene::beginAddingStem(const TagComponent& tag)
{
	lua_getglobal(m_LuaState, "Yeastem");
	std::string NodePath(tag.NodePath);
	NodePath += '/';
	int hierarchyLevel = 1;
	while (NodePath.find('/') != std::string::npos)
	{
		hierarchyLevel++;
		std::string nodeName = NodePath.substr(0, NodePath.find('/'));
		NodePath = NodePath.substr(NodePath.find('/') + 1);
		lua_getfield(m_LuaState, -1, nodeName.c_str());
		if (lua_isnil(m_LuaState, -1))
		{
			lua_pop(m_LuaState, 1);
			lua_pushstring(m_LuaState, nodeName.c_str());
			lua_newtable(m_LuaState);
			lua_settable(m_LuaState, -3);
			lua_getfield(m_LuaState, -1, nodeName.c_str());
		}
	}
	return hierarchyLevel;
}

void LuaScene::addStem(const TransformComponent& transform)
{
	LuaVector2::InitVector(transform.Position, m_LuaState);
	lua_setfield(m_LuaState, -2, "Position");

	lua_pushnumber(m_LuaState, transform.Rotation);
	lua_setfield(m_LuaState, -2, "Rotation");

	lua_pushnumber(m_LuaState, transform.Scale);
	lua_setfield(m_LuaState, -2, "Scale");
}

void LuaScene::getStem(TransformComponent& transform)
{
	lua_getfield(m_LuaState, -1, "Position");
	LuaVector2::UpdateVector(transform.Position, m_LuaState);
	lua_pop(m_LuaState, 1);

	lua_getfield(m_LuaState, -1, "Rotation");
	transform.Rotation = (float)lua_tonumber(m_LuaState, -1);
	lua_pop(m_LuaState, 1);

	lua_getfield(m_LuaState, -1, "Scale");
	transform.Scale = (float)lua_tonumber(m_LuaState, -1);
	lua_pop(m_LuaState, 1);
}

void LuaScene::addStem(const RenderQuadComponent& render)
{
	lua_pushnumber(m_LuaState, render.Size.x);
	lua_setfield(m_LuaState, -2, "width");

	lua_pushnumber(m_LuaState, render.Size.y);
	lua_setfield(m_LuaState, -2, "height");
}

void LuaScene::PrintGlobal(const char* gobal)
{
	lua_getglobal(m_LuaState, gobal);
	std::cout << gobal << " ";
	m_LuaState.PrintLuaValue(-1, 0);
	lua_pop(m_LuaState, 1);
}

void LuaScene::UpdateWindow(const ImVec2& windowSize)
{
	lua_getglobal(m_LuaState, "Window");

	lua_pushnumber(m_LuaState, windowSize.x);
	lua_setfield(m_LuaState, -2, "width");

	lua_pushnumber(m_LuaState, windowSize.y);
	lua_setfield(m_LuaState, -2, "height");

	lua_pop(m_LuaState, 1);
}

void LuaScene::addKeyEnum(const char* keyName, uint32_t keyCode)
{
	lua_pushstring(m_LuaState, keyName);
	lua_pushnumber(m_LuaState, keyCode);
	lua_settable(m_LuaState, -3);
}

void LuaScene::AttachNativeFunction(NativeFunction nativeFunction, const char* functionName)
{
	m_LuaState.AddNativeFunction(nativeFunction, functionName);
}

void LuaScene::ExecuteScript(ScriptComponent& scriptComponent, TagComponent& tagComponent)
{
	if (!FileIO::checkIfExists(scriptComponent.FilePath)) return;
	m_LuaState.ExecuteScript(scriptComponent, tagComponent);
}

void LuaScene::Recreate()
{
	m_LuaState.CreateNewState();
	Init();
}

YEASTEM_END
