#include "yst_pch.h"

#include "LuaScene.h"

#include "Yeastem/Core/Application.h"
#include "Scene.h"

#include "Yeastem/Core/Lua/LuaPanel.h"
#include "Yeastem/Core/Lua/LuaVector2.h"

#include "Yeastem/FileIO/FileIO.h"

#include "Components.h"

YEASTEM_BEGIN

void LuaScene::Init()
{
	luaL_openlibs(m_LuaState);

	lua_newtable(m_LuaState);
	lua_setglobal(m_LuaState, "Yeastem");

	luaL_newmetatable(m_LuaState, "Tree_t");
	lua_pushstring(m_LuaState, "__index");

	lua_getglobal(m_LuaState, "Yeastem");
	lua_pushstring(m_LuaState, "Tree");
	lua_newtable(m_LuaState);
	lua_settable(m_LuaState, -3);

	LuaImguiPanel::Init(m_LuaState);
	LuaVector2::Init(m_LuaState);

	lua_newtable(m_LuaState);
	lua_pushstring(m_LuaState, "IsKeyDown");
	lua_pushcfunction(m_LuaState, [](lua_State* L) -> int {
		if (lua_isnumber(L, -1))
		{
			uint32_t keyID = (uint32_t)lua_tonumber(L, -1);
			lua_pushboolean(L, Application::Get().GetEventHandler().IsKeyDown(keyID));
		}
		else if (lua_isstring(L, -1))
		{
			char key = std::tolower(lua_tostring(L, -1)[0]);
			lua_pushboolean(L, Application::Get().GetEventHandler().IsKeyDown(key));
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

void LuaScene::addStem(const TransformComponent& transform)
{
	LuaVector2::InitVector(transform.Position, m_LuaState);
	lua_setfield(m_LuaState, -2, "position");

	lua_pushnumber(m_LuaState, transform.Rotation);
	lua_setfield(m_LuaState, -2, "rotation");

	LuaVector2::InitVector(transform.Scale, m_LuaState);
	lua_setfield(m_LuaState, -2, "scale");
}

void LuaScene::getStem(TransformComponent& transform)
{
	lua_getfield(m_LuaState, -1, "position");
	LuaVector2::UpdateVector(transform.Position, m_LuaState);
	lua_pop(m_LuaState, 1);

	lua_getfield(m_LuaState, -1, "rotation");
	transform.Rotation = (float)lua_tonumber(m_LuaState, -1);
	lua_pop(m_LuaState, 1);

	lua_getfield(m_LuaState, -1, "scale");
	LuaVector2::UpdateVector(transform.Scale, m_LuaState);
	lua_pop(m_LuaState, 1);
}

void LuaScene::addStem(const RenderQuadComponent& render)
{
	LuaVector2::InitVector(render.Size, m_LuaState);
	lua_setfield(m_LuaState, -2, "size");
}

void LuaScene::getStem(RenderQuadComponent& render)
{
	lua_getfield(m_LuaState, -1, "size");
	LuaVector2::UpdateVector(render.Size, m_LuaState);
	lua_pop(m_LuaState, 1);
}

void LuaScene::PrintGlobal(const char* gobal)
{
	lua_getglobal(m_LuaState, gobal);
	std::cout << gobal << " ";
	m_LuaState.PrintLuaValue(-1, 0);
	lua_pop(m_LuaState, 1);
}

void LuaScene::UpdateWindow(const Vector2i& windowSize)
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

void LuaScene::ExecuteScript(const ScriptComponent& scriptComponent, const TagComponent& tagComponent)
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
