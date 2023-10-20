#pragma once

YEASTEM_BEGIN

class LuaState
{
public:
	LuaState() { m_State = luaL_newstate(); }

	LuaState(lua_State& state) :m_State(&state) {}

	LuaState(lua_State* state) :m_State(state) {}
	LuaState(lua_State* state, bool handleOwnership)
		:m_State(state), m_HandleOwnership(handleOwnership) {}

public:
	lua_State* Get() { return m_State; }
	operator lua_State* () { return m_State; }

public:
	bool executeScriptFromFile(const std::string& file);
	void addNativeFunction(int(*)(lua_State*), const char* functionName);

public:
	~LuaState() { if (m_HandleOwnership) lua_close(m_State); }

private:
	lua_State* m_State;
	bool m_HandleOwnership = true;
};

YEASTEM_END
