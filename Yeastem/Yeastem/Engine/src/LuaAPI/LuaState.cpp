#include "yst_pch.h"

#include "LuaState.h"

#include "FileIO/FileIO.h"

YEASTEM_BEGIN

bool LuaState::LoadScript(const char* script)
{
	int ID = luaL_loadstring(m_State, script);
	if (ID == LUA_OK) return true;

	const char* msg = lua_tostring(m_State, -1);
	lua_writestringerror("%s: ", "Lua");
	lua_writestringerror("%s\n", msg);
	lua_pop(m_State, 1);
	return false;
}

#if 1

void LuaState::PrintLuaTable(int index, uint32_t indentationLevel)
{
	int absIndex = lua_absindex(m_State, index);

	lua_pushnil(m_State);
	while (lua_next(m_State, absIndex) != 0)
	{
		// key is at -2, value is at -1
		if (lua_type(m_State, -2) == LUA_TSTRING)
		{
			const char* key = lua_tostring(m_State, -2);

			for (uint32_t i = 0; i < indentationLevel; ++i)
				std::cout << "\t";

			if (lua_istable(m_State, -1))
			{
				std::cout << key << " (table):\n";
				PrintLuaTable(-1, indentationLevel + 1);
			}
			else
			{
				std::cout << key << ": ";
				PrintLuaValue(-1, 0);
			}
		}

		lua_pop(m_State, 1);
	}
}

void LuaState::PrintLuaValue(int index, uint32_t indentationLevel)
{
	int type = lua_type(m_State, index);

	for (uint32_t i = 0; i < indentationLevel; ++i)
		std::cout << "\t";

	switch (type)
	{
	case LUA_TTABLE:
		std::cout << "(table):\n";
		PrintLuaTable(index, indentationLevel + 1);
		break;
	case LUA_TNUMBER:
		std::cout << "(number): " << lua_tonumber(m_State, index) << "\n";
		break;
	case LUA_TSTRING:
		std::cout << "(string): " << lua_tostring(m_State, index) << "\n";
		break;
	case LUA_TBOOLEAN:
		std::cout << "(bool): " << (lua_toboolean(m_State, index) ? "true" : "false") << "\n";
		break;
	case LUA_TFUNCTION:
		std::cout << "(function)\n";
		break;
	case LUA_TNIL:
		std::cout << "(nil)\n";
		break;
	default:
		std::cout << "(type " << luaL_typename(m_State, index) << ")\n";
		break;
	}
}

#else
void PrintLuaTable(lua_State* L, int index, uint32_t indentationLevel = 0)
{
	if (!lua_istable(L, index))
	{
		YEASTEM_WARNING("Not a Lua table at index " << index);
		return;
	}

	int absIndex = lua_absindex(L, index);

	lua_pushnil(L);
	while (lua_next(L, absIndex) != 0)
	{
		// key is at -2, value is at -1
		if (lua_type(L, -2) == LUA_TSTRING)
		{
			const char* key = lua_tostring(L, -2);

			for (uint32_t i = 0; i < indentationLevel; ++i)
				std::cout << "\t";

			switch (lua_type(L, -1))
			{
			case LUA_TTABLE:
				std::cout << key << " (table):\n";
				PrintLuaTable(L, -1, indentationLevel + 1);
				break;
			case LUA_TNUMBER:
				std::cout << key << " (number): " << lua_tonumber(L, -1) << "\n";
				break;
			case LUA_TSTRING:
				std::cout << key << " (string): " << lua_tostring(L, -1) << "\n";
				break;
			case LUA_TBOOLEAN:
				std::cout << key << " (bool): " << (lua_toboolean(L, -1) ? "true" : "false") << "\n";
				break;
			case LUA_TFUNCTION:
				std::cout << key << " (function)\n";
				break;
			case LUA_TNIL:
				std::cout << key << " (nil)\n";
				break;
			default:
				std::cout << key << " (type " << luaL_typename(L, -1) << ")\n";
				break;
			}
		}

		lua_pop(L, 1); // pop value, keep key
	}
}
#endif

bool LuaState::ExecuteScriptModifyEnv(const char* script, const ScriptComponent& scriptComponent, const TagComponent& tagComponent)
{
	lua_getglobal(m_State, "Yeastem");
	if (lua_isnil(m_State, -1))
	{
		YEASTEM_ERROR("Yeastem Lua Not Initiated with values!");
		return false;
	}

	std::string NodePath(tagComponent.NodePath);
	NodePath += '/';
	int hierarchyLevel = 1;
	while (NodePath.find('/') != std::string::npos)
	{
		hierarchyLevel++;
		std::string nodeName = NodePath.substr(0, NodePath.find('/'));
		NodePath = NodePath.substr(NodePath.find('/') + 1);
		lua_getfield(m_State, -1, nodeName.c_str());

		if (lua_isnil(m_State, -1))
		{
			lua_pop(m_State, 1);
			lua_pushstring(m_State, nodeName.c_str());
			lua_newtable(m_State);
			lua_settable(m_State, -3);

			lua_getfield(m_State, -1, nodeName.c_str());
		}
	}

	int EnvTableIndex = lua_gettop(m_State);

	lua_getglobal(m_State, "_G");
	lua_newtable(m_State);
	lua_pushvalue(m_State, -2);
	lua_setfield(m_State, -2, "__index");
	lua_setmetatable(m_State, EnvTableIndex);
	lua_pop(m_State, 1);

	bool success = LoadScript(script);

	if (!lua_isfunction(m_State, -1))
	{
		YEASTEM_ERROR("Expected Lua function after loading script.");
		lua_pop(m_State, 1);
		return false;
	}

	lua_pushvalue(m_State, EnvTableIndex);
	if (!lua_setupvalue(m_State, -2, 1))
	{
		YEASTEM_ERROR("Failed to set lua upvalue for script: " << scriptComponent.FilePath);
		lua_pop(m_State, 1);
		return false;
	}

	if (lua_pcall(m_State, 0, LUA_MULTRET, 0) != LUA_OK)
	{
		const char* msg = lua_tostring(m_State, -1);
		lua_writestringerror("Lua Error: %s\n", msg);
		lua_pop(m_State, 1);
		return false;
	}

	return success;
}

bool LuaState::ExecuteScript(const ScriptComponent& scriptComponent, const TagComponent& tagComponent)
{
	const std::string& str = FileIO::readFileText(scriptComponent.FilePath);
	return ExecuteScriptModifyEnv(str.c_str(), scriptComponent, tagComponent);
}

void LuaState::AddNativeFunction(int(*nativeFunction)(lua_State*), const char* functionName)
{
	lua_pushcfunction(m_State, nativeFunction);
	lua_setglobal(m_State, functionName);
}

void LuaState::CreateNewState()
{
	lua_close(m_State);
	m_State = luaL_newstate();
}

YEASTEM_END
