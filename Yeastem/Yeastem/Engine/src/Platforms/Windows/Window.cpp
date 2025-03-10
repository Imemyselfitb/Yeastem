#include "yst_pch.h"

#include "Core/Math/Vector2.h"

#ifdef YST_PLATFORM_WINDOWS
#include "Window.h"
#include <SDL_syswm.h>

YEASTEM_BEGIN

bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey)
{    
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam)
{
    int length = GetWindowTextLength(hWnd);
    WCHAR* wstr_buffer = new WCHAR[length + 1];
    GetWindowText(hWnd, wstr_buffer, length + 1);
    size_t convertedChars = 0;

    CHAR* str_buffer = new CHAR[length + 1];
    errno_t err = wcstombs_s(&convertedChars, str_buffer, length + 1, wstr_buffer, length);

    if (err == 0) {
        std::string str(str_buffer);
        std::cout << str_buffer << std::endl; // Output: Hello, world!
    }
    else {
        std::cerr << "Conversion error" << std::endl;
    }

    str_buffer[length] = '\0'; // Add null terminator

    std::string windowTitle(str_buffer);

    delete[] str_buffer;
    delete[] wstr_buffer;

    // List visible windows with a non-empty title
    if (IsWindowVisible(hWnd) && length != 0)
    {
        Vector2 _WindowPos_;
        Vector2 _WindowSize_;

        RECT WindowBounds;
        if (GetWindowRect(hWnd, &WindowBounds))
        {
            _WindowPos_.x = static_cast<float>(WindowBounds.left);
            _WindowPos_.y = static_cast<float>(WindowBounds.top);
            _WindowSize_.x = static_cast<float>(WindowBounds.right - WindowBounds.left);
            _WindowSize_.y = static_cast<float>(WindowBounds.bottom - WindowBounds.top);
        }

        AllWindows.emplace_back(windowTitle, _WindowPos_, _WindowSize_);
    }
    return TRUE;
}

int GetWindowData()
{
    std::cout << "ARGHHHH\n";
    AllWindows.clear();
    EnumWindows(Yeastem::enumWindowCallback, NULL);
    return 1;
}

YEASTEM_END
#endif
