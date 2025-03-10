#pragma once

#include "Application.h"
#include "Platforms/Windows/Window.h"

YEASTEM_BEGIN
int EntryPoint(int argc, char* argv[])
{
	//static std::ofstream out("LOG.txt");
	//std::cout.rdbuf(out.rdbuf());

	//GetWindowData();

	Application::SetupGLFlags();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not be initialized: " << SDL_GetError();
	}

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow(
		"Yeastem!", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		1350, 600, flags
	);

#if YEASTEM_COLOURLESS
	MakeWindowTransparent(window, RGB(255,0,255));
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
#endif

	Application::AttachWindow(window);
	Application::CreateGLContext();
	Application::InitImGui();

	Application::Run();

	for (auto& window : AllWindows)
	{
		std::cout << ">>>" << window.Title << "\n";
		std::cout << "(" << window.Position << ")" << "\n";
		std::cout << "(" << window.Size << ")" << "\n";
	}

	Application::DestroyWindow();

	SDL_Quit();

	return 0;
}

YEASTEM_END
