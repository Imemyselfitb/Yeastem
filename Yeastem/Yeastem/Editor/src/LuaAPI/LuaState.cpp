#include "yst_pch.h"

#include "Lua.h"

YEASTEM_BEGIN
bool LuaState::executeScriptFromFile(const std::string& file)
{
	const std::string& str = readFileText(file);
	const char* cStr = str.c_str();
	int ID = lua_pcall(this->m_State, 0, -1, 0);
	if (ID == LUA_OK) ID = luaL_loadstring(this->m_State, cStr);
	if (ID == LUA_OK) return true;

	const char* msg = lua_tostring(this->m_State, -1);
	lua_writestringerror("%s: ", "Lua");
	lua_writestringerror("%s\n", msg);
	lua_pop(this->m_State, 1);
	return false;
}

void LuaState::addNativeFunction(int(*new_function)(lua_State*), const char* function_name)
{
	lua_pushcfunction(this->m_State, new_function);
	lua_setglobal(this->m_State, function_name);
}

YEASTEM_END
