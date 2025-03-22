#include "yst_pch.h"

#include "LuaPanel.h"

YEASTEM_BEGIN

void LuaImguiPanel::Init(LuaState& L)
{
	lua_getglobal(L, "Yeastem");
	lua_pushstring(L, "__Panels__");

	lua_newtable(L);
	lua_pushstring(L, "__count");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);

	lua_settable(L, -3);
	lua_pop(L, 1);

	lua_newtable(L);
	int panelTableIdx = lua_gettop(L);
	lua_pushvalue(L, panelTableIdx);
	lua_setglobal(L, "Panel");

	lua_pushstring(L, "new");
	lua_pushcfunction(L, LuaImguiPanel::__new);
	lua_settable(L, -3);

	lua_pushstring(L, "createElement");
	lua_pushcfunction(L, LuaImguiPanel::__createElement);
	lua_settable(L, -3);

	lua_pushstring(L, "AppendChild");
	lua_pushcfunction(L, LuaImguiPanel::__appendChild);
	lua_settable(L, -3);

	lua_pushstring(L, "Submit");
	lua_pushcfunction(L, LuaImguiPanel::__Submit);
	lua_settable(L, -3);

	luaL_newmetatable(L, "Panel_t");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, panelTableIdx);
	lua_settable(L, -3);

	luaL_newmetatable(L, "PanelElement_t");
}

void LuaImguiPanel::ShowAll(LuaState& L)
{
	lua_getglobal(L, "Yeastem");
	lua_getfield(L, -1, "__Panels__");

	lua_getfield(L, -1, "__count");
	int count = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	for (int i = 0; i < count; i++)
	{
		lua_rawgeti(L, -1, i);
		LuaImguiPanel::ShowElement(L);
	}

	lua_pop(L, 2);
}

void LuaImguiPanel::ShowElement(LuaState& L)
{
	lua_getfield(L, -1, "ClassName");
	std::string Class = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "Name");
	const char* Name = lua_tostring(L, -1);
	lua_pop(L, 1);

	if (Class == "Panel")
	{
		ImGui::Begin(Name);
		
		lua_getfield(L, -1, "Contents");

		lua_getfield(L, -1, "__count");
		lua_Number count = lua_tonumber(L, -1);
		lua_pop(L, 1);

		for (int i = 0; i < count; i++)
		{
			lua_rawgeti(L, -1, i);
			LuaImguiPanel::ShowElement(L);
		}

		lua_pop(L, 1);

		ImGui::End();
	}
	else if (Class == "PanelElement")
	{
		lua_getfield(L, -1, "Type");
		std::string Type = lua_tostring(L, -1);
		lua_pop(L, 1);

		if (Type == "Button")
		{
			bool clicked = ImGui::Button(Name);
			bool active = ImGui::IsItemActive();

			lua_getfield(L, -1, "IsMouseDown");
			bool btn_state = (lua_isboolean(L, -1)) ? lua_toboolean(L, -1) : 0;
			lua_pop(L, 1);

			if (clicked)
			{
				lua_getfield(L, -1, "OnClick");
				if (lua_isfunction(L, -1))
					lua_pcall(L, 0, 0, 0);
				else 
					lua_pop(L, 1);
			}

			if (!btn_state && active)
			{
				lua_getfield(L, -1, "OnMouseDown");
				if (lua_isfunction(L, -1))
					lua_pcall(L, 0, 0, 0);
				else
					lua_pop(L, 1);
			}

			if (btn_state && !active)
			{
				lua_getfield(L, -1, "OnMouseUp");
				if (lua_isfunction(L, -1))
					lua_pcall(L, 0, 0, 0);
				else
					lua_pop(L, 1);
			}

			lua_pushboolean(L, active);
			lua_setfield(L, -2, "IsMouseDown");
		}
		else if (Type == "Text")
		{
			ImGui::TextWrapped(Name);
		}
	}

	lua_pop(L, 1);
}

int LuaImguiPanel::__new(lua_State* L)
{
	if (!lua_isstring(L, -1)) return 0;

	const char* name = lua_tostring(L, -1);

	lua_newtable(L);

	lua_pushstring(L, "ClassName");
	lua_pushstring(L, "Panel");
	lua_settable(L, -3);

	lua_pushstring(L, "Name");
	lua_pushstring(L, name);
	lua_settable(L, -3);

	lua_pushstring(L, "Contents");
	lua_newtable(L);

	lua_pushstring(L, "__count");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);

	lua_settable(L, -3); // Push Contents into Object

	luaL_getmetatable(L, "Panel_t");
	lua_setmetatable(L, -2);

	return 1;
}

int LuaImguiPanel::__appendChild(lua_State* L)
{
	if (!lua_istable(L, -2)) return 0;

	lua_getfield(L, -2, "ClassName");
	if (!lua_isstring(L, -1))
	{
		lua_pop(L, 1);
		return 0;
	}

	std::string typeoffirstparam = lua_tostring(L, -1);
	if (!lua_isstring(L, -1) || !(typeoffirstparam == "Panel" || typeoffirstparam == "PanelElement"))
	{
		lua_pop(L, 1);
		return 0;
	}
	lua_pop(L, 1);

	if (lua_istable(L, -1))
	{
		lua_getfield(L, -1, "ClassName");
		if (!lua_isstring(L, -1) || (std::string)lua_tostring(L, -1) != (std::string)"PanelElement")
		{
			lua_pop(L, 1);
			return 0;
		}
		lua_pop(L, 1);

		lua_getfield(L, -2, "Contents");

		lua_getfield(L, -1, "__count");
		lua_Number __count = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_pushnumber(L, __count);
		lua_pushvalue(L, -3);
		lua_settable(L, -3);

		lua_pushstring(L, "__count");
		lua_pushnumber(L, __count + 1);
		lua_settable(L, -3);
		
		lua_pop(L, 1);
	}

	return 0;
}

int LuaImguiPanel::__createElement(lua_State* L)
{
	if (!lua_isstring(L, -2) || !lua_isstring(L, -1)) return 0;

	const char* type = lua_tostring(L, -2);
	const char* name = lua_tostring(L, -1);

	lua_newtable(L);
	lua_pushstring(L, "ClassName");
	lua_pushstring(L, "PanelElement");
	lua_settable(L, -3);

	lua_pushstring(L, "Type");
	lua_pushstring(L, type);
	lua_settable(L, -3);

	lua_pushstring(L, "Name");
	lua_pushstring(L, name);
	lua_settable(L, -3);

	lua_pushstring(L, "AppendChild");
	lua_pushcfunction(L, LuaImguiPanel::__appendChild);
	lua_settable(L, -3);

	lua_pushstring(L, "Contents");
	lua_newtable(L);

	lua_pushstring(L, "__count");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);

	lua_settable(L, -3); // Push Contents into Object

	luaL_getmetatable(L, "PanelElement_t");
	lua_setmetatable(L, -2);

	return 1;
}

int LuaImguiPanel::__Submit(lua_State* L)
{
	if (!lua_istable(L, -1)) return 0;
	lua_getfield(L, -1, "ClassName");
	if (!lua_isstring(L, -1) || (std::string)lua_tostring(L, -1) != "Panel")
	{
		lua_pop(L, 1);
		return 0;
	}
	lua_pop(L, 1);

	int panelIdx = lua_gettop(L);

	lua_getglobal(L, "Yeastem");
	lua_getfield(L, -1, "__Panels__");
	
	lua_getfield(L, -1, "__count");
	lua_Number count = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushnumber(L, count);
	lua_pushvalue(L, panelIdx);
	lua_settable(L, -3);

	lua_pushstring(L, "__count");
	lua_pushnumber(L, count + 1);
	lua_settable(L, -3);

	lua_pop(L, 2);
	return 0;
}

YEASTEM_END
