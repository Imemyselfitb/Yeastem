#include "yst_pch.h"

#include "LuaVector2.h"

// Metatable Methods: https://gist.github.com/oatmealine/655c9e64599d0f0dd47687c1186de99f

YEASTEM_BEGIN

bool LuaVector2::m_Initialised = false;

void LuaVector2::Init(LuaState& L)
{
	if (m_Initialised) return;

	lua_newtable(L);

	lua_pushstring(L, "new");
	lua_pushcfunction(L, LuaVector2::__new);
	lua_settable(L, -3);

	lua_setglobal(L, "Vector2");
	luaL_newmetatable(L, "Vector2MetaTable");

	lua_pushstring(L, "__add");
	lua_pushcfunction(L, LuaVector2::__add);
	lua_settable(L, -3);
	lua_pushstring(L, "__sub");
	lua_pushcfunction(L, LuaVector2::__sub);
	lua_settable(L, -3);

	lua_pushstring(L, "__mul");
	lua_pushcfunction(L, LuaVector2::__multiply);
	lua_settable(L, -3);
	lua_pushstring(L, "__div");
	lua_pushcfunction(L, LuaVector2::__divide);
	lua_settable(L, -3);

	m_Initialised = true;
}

void LuaVector2::InitVector(const Vector2& v, lua_State* L)
{
	lua_newtable(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, v.x);
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, v.y);
	lua_settable(L, -3);

	luaL_getmetatable(L, "Vector2MetaTable");
	lua_setmetatable(L, -2);
}

void LuaVector2::UpdateVector(Vector2& v, LuaState& L)
{
	lua_getfield(L, -1, "x");
	v.x = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	v.y = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);
}

int LuaVector2::__new(lua_State* L)
{
	float x = 0, y = 0;
	if (lua_isnumber(L, -2) && lua_isnumber(L, -1))
	{
		x = lua_tonumber(L, -2);
		y = lua_tonumber(L, -1);
	}
	else if (lua_istable(L, -1))
	{
		lua_getfield(L, -1, "x");
		x = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		y = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}

	lua_newtable(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x);
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, y);
	lua_settable(L, -3);

	luaL_getmetatable(L, "Vector2MetaTable");
	lua_setmetatable(L, -2);

	return 1;
}

int LuaVector2::__add(lua_State* L)
{
	if (!lua_istable(L, -1) || !lua_istable(L, -2)) return 0;
	
	lua_getfield(L, -2, "x");
	float x1 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -2, "y");
	float y1 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_getfield(L, -1, "x");
	float x2 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	float y2 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVector2::InitVector(Vector2(x1 + x2, y1 + y2), L);

	return 1;
}

int LuaVector2::__sub(lua_State* L)
{
	if (!lua_istable(L, -1) || !lua_istable(L, -2)) return 0;

	lua_getfield(L, -2, "x");
	float x1 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -2, "y");
	float y1 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "x");
	float x2 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	float y2 = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVector2::InitVector(Vector2(x1 - x2, y1 - y2), L);

	return 1;
}

int LuaVector2::__multiply(lua_State* L)
{
	if (lua_istable(L, -1) && lua_istable(L, -2))
	{
		lua_getfield(L, -2, "x");
		float x1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -2, "y");
		float y1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "x");
		float x2 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		float y2 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		LuaVector2::InitVector(Vector2(x1 * x2, y1 * y2), L);
		return 1;
	}

	if (lua_isnumber(L, -1) || lua_isnumber(L, -2))
	{
		int sfIdx, tableIdx;
		if (lua_isnumber(L, -1))
		{
			sfIdx = -1;
			tableIdx = -2;
		}
		else
		{
			sfIdx = -2;
			tableIdx = -1;
		}

		float sf = (float)lua_tonumber(L, sfIdx);

		lua_getfield(L, tableIdx, "x");
		float x1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, tableIdx, "y");
		float y1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		LuaVector2::InitVector(Vector2(x1 * sf, y1 * sf), L);
	}

	return 1;
}

int LuaVector2::__divide(lua_State* L)
{
	if (lua_istable(L, -1) && lua_istable(L, -2))
	{
		lua_getfield(L, -2, "x");
		float x1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -2, "y");
		float y1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "x");
		float x2 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		float y2 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		LuaVector2::InitVector(Vector2(x1 / x2, y1 / y2), L);
		return 1;
	}

	if (lua_isnumber(L, -1) || lua_isnumber(L, -2))
	{
		int sfIdx, tableIdx;
		if (lua_isnumber(L, -1))
		{
			sfIdx = -1;
			tableIdx = -2;
		}
		else
		{
			sfIdx = -2;
			tableIdx = -1;
		}

		float sf = (float)lua_tonumber(L, sfIdx);

		lua_getfield(L, tableIdx, "x");
		float x1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, tableIdx, "y");
		float y1 = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		if(sfIdx == -1)
			LuaVector2::InitVector(Vector2(x1 / sf, y1 / sf), L);
		else
			LuaVector2::InitVector(Vector2(sf / x1, sf / y1), L);
	}

	return 1;
}

YEASTEM_END
