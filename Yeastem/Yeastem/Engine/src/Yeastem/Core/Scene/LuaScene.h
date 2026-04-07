#pragma once

#include "Components.h"
#include "Yeastem/LuaAPI/LuaState.h"

YEASTEM_BEGIN

class LuaScene
{
public:
	using NativeFunction = int(*)(lua_State*);

public:
	void Init();
	LuaState& GetState() { return m_LuaState; }

	void AttachNativeFunction(NativeFunction nativeFunction, const char* functionName);
	void ExecuteScript(const ScriptComponent& scriptComponent, const TagComponent& tagComponent);
	void Recreate();

	template<typename... Args>
	void CallYeastemFunction(const TagComponent& tagComponent, const char* functionName, Args... args);

	template<typename Component>
	void SetComponentToScript(const Component& component, const TagComponent& tag)
	{
		int hierachyLevel = m_LuaState.SetupObjectTree(tag);
		addStem(component);
		lua_pop(m_LuaState, hierachyLevel);
	}

	template<typename Component>
	void UpdateComponentFromScript(Component& component, const TagComponent& tag)
	{
		int hierachyLevel = m_LuaState.SetupObjectTree(tag);
		getStem(component);
		lua_pop(m_LuaState, hierachyLevel);
	}

	void PrintGlobal(const char* varName);
	void UpdateWindow(const Vector2i& windowSize);

private:
	void addKeyEnum(const char* keyName, uint32_t keyCode);

	void addStem(const TransformComponent& transform);
	void addStem(const RenderQuadComponent& render);

	void getStem(TransformComponent& transform);
	void getStem(RenderQuadComponent& render);

private:
	LuaState m_LuaState;
};


template<typename... Args>
inline void LuaScene::CallYeastemFunction(const TagComponent& tagComponent, const char* functionName, Args... args)
{
	lua_State* L = m_LuaState;

	int hierarchyLevel = m_LuaState.SetupScriptCall(tagComponent);

	lua_getfield(L, -1, functionName);

	if (lua_isfunction(L, -1))
	{
		(..., lua_pushnumber(L, args));
		if (LUA_OK != lua_pcall(L, 1, 0, 0))
		{
			const char* msg = lua_tostring(L, -1);
			lua_writestringerror("%s: ", "Lua");
			lua_writestringerror("%s\n", msg);
			lua_pop(L, 1);
		}
	}
	else
		lua_pop(L, 1);

	lua_pop(L, hierarchyLevel);
}

YEASTEM_END
