#pragma once

#include <SDL.h>
#include <glad/glad.h>

#include <iostream>

#include "Macros.h"
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

	Application::Run(); 

	Application::DestroyWindow();

	return 1;
}
YEASTEM_END