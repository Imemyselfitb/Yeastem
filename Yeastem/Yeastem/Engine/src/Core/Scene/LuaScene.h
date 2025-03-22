#pragma once

#include "Core/Scene/Components.h"

#include "LuaAPI/LuaState.h"

#include "Core/OpenGL/Renderer.h"

YEASTEM_BEGIN

class LuaScene
{
public:
	using NativeFunction = int(*)(lua_State*);

public:
	void Init();
	LuaState& GetState() { return m_LuaState; }

	void AttachNativeFunction(NativeFunction nativeFunction, const char* functionName);
	void ExecuteScript(ScriptComponent& scriptComponent, TagComponent& tagComponent);
	void Recreate();

	template<typename... Args>
	void CallYeastemFunction(TagComponent& tagComponent, const char* functionName, Args... args);

	template<typename Component>
	void SetComponent(const Component& component, const TagComponent& tag)
	{
		int hierachyLevel = beginAddingStem(tag);
		addStem(component);
		lua_pop(m_LuaState, hierachyLevel);
	}

	template<typename Component>
	void GetComponent(Component& component, const TagComponent& tag)
	{
		int hierachyLevel = beginAddingStem(tag);
		getStem(component);
		lua_pop(m_LuaState, hierachyLevel);
	}

	void PrintGlobal(const char* varName);
	void UpdateWindow(const ImVec2& windowSize);

private:
	void addKeyEnum(const char* keyName, uint32_t keyCode);
	int beginAddingStem(const TagComponent& tag);
	void addStem(const TransformComponent& transform);
	void getStem(TransformComponent& transform);
	void addStem(const RenderQuadComponent& render);

private:
	LuaState m_LuaState;
};


template<typename... Args>
inline void LuaScene::CallYeastemFunction(TagComponent& tagComponent, const char* functionName, Args... args)
{
	lua_State* L = m_LuaState;
	lua_getglobal(m_LuaState, "Yeastem");
	std::string NodePath(tagComponent.NodePath);
	NodePath += '/';
	int hierarchyLevel = 1;
	while (NodePath.find('/') != std::string::npos)
	{
		hierarchyLevel++;
		std::string nodeName = NodePath.substr(0, NodePath.find('/'));
		NodePath = NodePath.substr(NodePath.find('/') + 1);
		lua_getfield(L, -1, nodeName.c_str());

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_pushstring(L, nodeName.c_str());
			lua_newtable(L);
			lua_settable(L, -3);

			lua_getfield(L, -1, nodeName.c_str());
		}
	}

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
