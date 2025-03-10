#include "yst_pch.h"

#include "Lua.h"

YEASTEM_BEGIN

bool LuaState::ExecuteScript(const std::string& script)
{
	int ID = luaL_dostring(m_State, script.c_str());
	if (ID == LUA_OK) return true;

	const char* msg = lua_tostring(m_State, -1);
	lua_writestringerror("%s: ", "Lua");
	lua_writestringerror("%s\n", msg);
	lua_pop(m_State, 1);
	return false;
}

int nextfile_uuid = 46290;

bool LuaState::ExecuteScript(const std::string& script, const std::string& fileName)
{
	std::regex exp("function Yeastem.(\\w)+( )*\\(");
	std::smatch res;
	std::string str = script;

	std::string::const_iterator searchStart(str.cbegin());

	std::vector<int> indices = {};
	std::vector<int> sizes = {};
	int c = 0;
	while (std::regex_search(searchStart, str.cend(), res, exp)) {
		c += (int)res.position();
		if (str.substr(c, 6) != "local ")
		{
			indices.push_back(c);
			sizes.push_back((int)res.length());
		}
		c += (int)res.length();
		searchStart = res.suffix().first;
	}

	nextfile_uuid++;

	int offset = 0;
	for (int i = 0; i < indices.size(); i++)
	{
		std::string toAppend = (std::string)"_" + std::to_string(nextfile_uuid);
		int funcDeclOffset = sizes[i] - 1;
		str = str.insert(indices[i] + funcDeclOffset + offset, toAppend);
		offset += (int)toAppend.size();
	}

	return ExecuteScript(str);
}

bool LuaState::executeScriptFromFile(const std::string& file)
{
	const std::string& str = FileIO::readFileText(file);
	return ExecuteScript(str, file);
}

void LuaState::addNativeFunction(int(*nativeFunction)(lua_State*), const char* functionName)
{
	lua_pushcfunction(m_State, nativeFunction);
	lua_setglobal(m_State, functionName);
}

void LuaState::CreateNewState()
{
	lua_close(m_State);
	m_State = luaL_newstate();
	nextfile_uuid = 46290;
}

YEASTEM_END
