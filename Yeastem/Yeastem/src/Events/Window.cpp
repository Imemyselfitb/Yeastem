#include "Window.h"

#include <memory>

YEASTEM_BEGIN

WindowEvent WindowEvent::s_Instance;

void WindowEvent::handle_events(const SDL_WindowEvent& e)
{
	if (this->AllWindowsData.find(e.windowID) == this->AllWindowsData.end())
	{
		WindowData wd;

		/* Setting up Window Data. */ {
			SDL_Window* window = SDL_GetWindowFromID(e.windowID);

			SDL_GetWindowPosition(window, &wd.left, &wd.top);
			SDL_GetWindowSize(window, &wd.width, &wd.height);

			delete window;
		}

		this->AllWindowsData[e.windowID] = wd;
		return;
	}

	WindowData& prevWindowData = this->AllWindowsData.at(e.windowID);

	switch (e.event)
	{
	case SDL_WINDOWEVENT_MOVED:
		if (prevWindowData.left != e.data1 || prevWindowData.top != e.data2)
		{
			prevWindowData.left = e.data1;
			prevWindowData.top = e.data2;
		}
	}
}

YEASTEM_END
