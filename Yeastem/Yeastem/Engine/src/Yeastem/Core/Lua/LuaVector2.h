#pragma once

#include "Yeastem/LuaAPI/LuaState.h"

YEASTEM_BEGIN

class LuaVector2
{
public:
	static void Init(lua_State* L);

	static void InitVector(const Vector2f& v, lua_State* state);
	static void UpdateVector(Vector2f& v, LuaState& state);

	static int __new(lua_State* state);
	static int __add(lua_State* state);
	static int __sub(lua_State* state);
	static int __divide(lua_State* state);
	static int __multiply(lua_State* state);
};

YEASTEM_END
