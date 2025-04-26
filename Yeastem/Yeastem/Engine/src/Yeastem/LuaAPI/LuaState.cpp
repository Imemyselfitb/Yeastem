#include "yst_pch.h"

#include "LuaState.h"

#include "Yeastem/FileIO/FileIO.h"

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

void LuaState::PrintLuaTable(int index, uint32_t indentationLevel)
{
	int absIndex = lua_absindex(m_State, index);

	// Check if table has already been visited
	void* tablePtr = (void*)lua_topointer(m_State, index);
	lua_pushlightuserdata(m_State, tablePtr);
	lua_rawget(m_State, LUA_REGISTRYINDEX);
	if (!lua_isnil(m_State, -1))
	{
		std::cout << "(table 0x" << std::hex << tablePtr << std::dec << ") [circular reference]\n";
		lua_pop(m_State, 1);
		return;
	}
	std::cout << "(table 0x" << std::hex << tablePtr << std::dec << "):\n";

	// Remove circular-reference check from stack
	lua_pop(m_State, 1);

	// Mark table as visited
	lua_pushlightuserdata(m_State, tablePtr);
	lua_pushboolean(m_State, 1);
	lua_rawset(m_State, LUA_REGISTRYINDEX);

	// Iterate over table
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
				std::cout << key << ": ";
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

	// Un-mark the table for next print() call
	lua_pushlightuserdata(m_State, tablePtr);
	lua_pushnil(m_State);
	lua_rawset(m_State, LUA_REGISTRYINDEX);
}

void LuaState::PrintLuaValue(int index, uint32_t indentationLevel)
{
	int type = lua_type(m_State, index);

	for (uint32_t i = 0; i < indentationLevel; ++i)
		std::cout << "\t";

	switch (type)
	{
	case LUA_TTABLE:
	{
		PrintLuaTable(index, indentationLevel + 1);
		break;
	}
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

int LuaState::SetupObjectTree(const TagComponent& tag)
{
	std::string NodePath = tag.NodePath + tag.Name + '/';

	lua_getglobal(m_State, "Yeastem");
	int hierarchyLevel = 1;
	while (NodePath.find('/') != std::string::npos)
	{
		hierarchyLevel++;
		std::string nodeName = NodePath.substr(0, NodePath.find('/'));
		NodePath = NodePath.substr(NodePath.find('/') + 1);
		lua_getfield(m_State, -1, nodeName.c_str());

		if (lua_isnil(m_State, -1))
		{
			// Remove nil value
			lua_pop(m_State, 1);

			// Create new table which is a child of the current node
			lua_newtable(m_State);
			lua_pushvalue(m_State, -2);
			lua_setfield(m_State, -2, "parent");
			
			// Assign node name
			lua_setfield(m_State, -2, nodeName.c_str());

			// Get child table for next iteration
			lua_getfield(m_State, -1, nodeName.c_str());
		}
	}

	return hierarchyLevel;
}

int LuaState::SetupScriptCall(const TagComponent& nodePath)
{
	int hierarchyLevel = SetupObjectTree(nodePath);
	lua_getglobal(m_State, "Yeastem");
	lua_pushvalue(m_State, -2);
	lua_setfield(m_State, -2, "script");
	lua_pop(m_State, 1);
	return hierarchyLevel;
}

bool LuaState::ExecuteScriptModifyEnv(const char* script, const ScriptComponent& scriptComponent, const TagComponent& tagComponent)
{
	SetupScriptCall(tagComponent);

	// Setup Environment table
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

	// Swap the Environment table's path to be relative to the script
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
