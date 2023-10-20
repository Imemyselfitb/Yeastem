#pragma once

#include "Application.h"

YEASTEM_BEGIN
int EntryPoint(int argc, char* argv[])
{
	Application::SetupGLFlags();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not be initialized: " << SDL_GetError;
	}

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow("Yeastem!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, flags);
	
	Application::AttachWindow(window);
	Application::CreateGLContext();
	Application::InitImGui();

	Application::Run(); 

	Application::DestroyWindow();

	SDL_Quit();

	return 1;
}

YEASTEM_END

#if 0

#include "LuaAPI/Lua.h"

YEASTEM_BEGIN
int EntryPoint(int argc, char* argv[])
{
	int(*print)(lua_State*) = [](lua_State* L) -> int
	{
		int top = lua_gettop(L);
		for (int i = 1; i <= top; i++) {
			int t = lua_type(L, i);
			switch (t)
			{
			case LUA_TSTRING:
				std::cout << lua_tostring(L, i);
				break;
			case LUA_TNUMBER:
				std::cout << lua_tonumber(L, i);
				break;
			case LUA_TBOOLEAN:
				std::cout << lua_toboolean(L, i);
				break;
			default:
				std::cout << lua_typename;
			}
			std::cout << " ";
		}
		std::cout << std::endl;
		return 0;
	};


#define addNativeLuaFunction(L, new_function)	\
			lua_pushcfunction(L, new_function); \
			lua_setglobal(L, #new_function);
	
	LuaState L;
	L.addNativeFunction(print, "print");
	L.executeScriptFromFile("test/Test.lua");

	lua_getglobal(L, "onready");
	if (lua_isfunction(L, -1))
	{
		std::cout << lua_pcall(L, 0, 0, 0);
	}

	std::cout << "AA";

#if 0
	lua_getglobal(L, "x");
	lua_Number x = lua_tonumber(L, 1);
	std::cout << "Lua says x = " << (int)x << std::endl;
#endif

	return 0;
}
YEASTEM_END

#endif
