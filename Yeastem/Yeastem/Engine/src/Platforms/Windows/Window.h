#pragma once

#ifdef YST_PLATFORM_WINDOWS
#undef APIENTRY
#include <windows.h>
YEASTEM_BEGIN

bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey);
int GetWindowData();

struct Windows
{
    inline Windows() {}
    inline Windows(const std::string& title, Vector2 position, Vector2 size)
        :Title(title), Position(position), Size(size)
    {
    }

    std::string Title;
    Vector2 Position;
    Vector2 Size;
};

inline std::vector<Windows> AllWindows;

YEASTEM_END
#endif
