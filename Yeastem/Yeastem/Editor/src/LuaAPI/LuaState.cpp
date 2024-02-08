#include "yst_pch.h"

#include "Lua.h"

YEASTEM_BEGIN

bool LuaState::ExecuteScript(const std::string& script)
{
	int ID = luaL_dostring(this->m_State, script.c_str());
	if (ID == LUA_OK) return true;

	const char* msg = lua_tostring(this->m_State, -1);
	lua_writestringerror("%s: ", "Lua");
	lua_writestringerror("%s\n", msg);
	lua_pop(this->m_State, 1);
	return false;
}

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

	static int uuid = 46290 - 1;
	uuid++;

	int offset = 0;
	for (int i = 0; i < indices.size(); i++)
	{
		std::string toAppend = (std::string)"_" + std::to_string(uuid);
		int funcDeclOffset = sizes[i] - 1;
		str = str.insert(indices[i] + funcDeclOffset + offset, toAppend);
		offset += (int)toAppend.size();
	}

	return this->ExecuteScript(str);
}

bool LuaState::executeScriptFromFile(const std::string& file)
{
	const std::string& str = readFileText(file);
	return this->ExecuteScript(str, file);
}

void LuaState::addNativeFunction(int(*nativeFunction)(lua_State*), const char* functionName)
{
	lua_pushcfunction(this->m_State, nativeFunction);
	lua_setglobal(this->m_State, functionName);
}

YEASTEM_END
