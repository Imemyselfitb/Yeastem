#pragma once

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
	bool ExecuteScript(const std::string& script);
	bool ExecuteScript(const std::string& script, const std::string& fileName);
	bool executeScriptFromFile(const std::string& file);
	void addNativeFunction(int(*nativeFunction)(lua_State*), const char* functionName);
	void CreateNewState();

public:
	~LuaState() { if (m_HandleOwnership) lua_close(m_State); }

private:
	lua_State* m_State;
	bool m_HandleOwnership = true;
};

YEASTEM_END
