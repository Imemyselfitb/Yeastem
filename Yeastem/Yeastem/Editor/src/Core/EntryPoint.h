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
