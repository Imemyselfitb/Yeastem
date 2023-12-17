#pragma once

#include "Application.h"

#if 1

YEASTEM_BEGIN
int EntryPoint(int argc, char* argv[])
{
	Application::SetupGLFlags();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not be initialized: " << SDL_GetError;
	}

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow("Yeastem!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 500, flags);
	
	Application::AttachWindow(window);
	Application::CreateGLContext();
	Application::InitImGui();

	Application::Run(); 

	Application::DestroyWindow();

	SDL_Quit();

	return 1;
}

YEASTEM_END

#else

#include "LuaAPI/Lua.h"

YEASTEM_BEGIN
int EntryPoint(int argc, char* argv[])
{
	LuaState L;
	L.executeScriptFromFile("test/Test.lua");

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
