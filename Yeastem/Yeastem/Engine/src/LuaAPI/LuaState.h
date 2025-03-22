#pragma once

#include "Lua.h"
#include "Core/Scene/Components.h"

YEASTEM_BEGIN

class LuaState
{
public:
	LuaState() { m_State = luaL_newstate(); }
	LuaState(const LuaState& other) :m_State(other.m_State) {}
	LuaState(LuaState&& other) noexcept :m_State(other.m_State) { other.m_HandleOwnership = false; }

	void operator=(const LuaState& other) { m_State = other.m_State; }
	void operator=(LuaState& other) { m_State = other.m_State; other.m_HandleOwnership = false; }

	LuaState(bool handleOwnership) :m_HandleOwnership(handleOwnership) { m_State = luaL_newstate(); }

	LuaState(lua_State* state) :m_State(state) {}
	LuaState(lua_State* state, bool handleOwnership)
		:m_State(state), m_HandleOwnership(handleOwnership) {}

public:
	lua_State* Get() { return m_State; }
	operator lua_State* () { return m_State; }

public:
	bool ExecuteScript(const ScriptComponent& scriptComponent, const TagComponent& tagComponent);
	void AddNativeFunction(int(*nativeFunction)(lua_State*), const char* functionName);
	void CreateNewState();
	void PrintLuaValue(int index, uint32_t indentationLevel = 0);

public:
	~LuaState() { if (m_HandleOwnership) lua_close(m_State); }

private:
	bool ExecuteScriptModifyEnv(const char* script, const ScriptComponent& scriptComponent, const TagComponent& tagComponent);
	bool LoadScript(const char* script);
	void PrintLuaTable(int index, uint32_t indentationLevel);

private:
	lua_State* m_State;
	bool m_HandleOwnership = true;
};

YEASTEM_END
