#pragma once

#include "Yeastem/LuaAPI/LuaState.h"

YEASTEM_BEGIN

class LuaImguiPanel
{
public:
	static void Init(LuaState& state);

	static int __new(lua_State* state);
	static int __appendChild(lua_State* state);
	static int __createElement(lua_State* state);
	static int __Submit(lua_State* state);
	static void ShowAll(LuaState& state);

private:
	static void ShowElement(LuaState& state);
};

YEASTEM_END
