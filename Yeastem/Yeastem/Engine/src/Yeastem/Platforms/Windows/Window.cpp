#include "yst_pch.h"

// Only compile if within windows
#ifdef YST_PLATFORM_WINDOWS

#include "Yeastem/Core/Math/Vector2.h"

#include "Window.h"

#undef APIENTRY
#include <Windows.h>

#include <SDL3/SDL.h>

YEASTEM_BEGIN

bool MakeWindowTransparent(SDL_Window* window, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetWindowOpacity(window, 0.0f);
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Fully transparent

    //HWND hWnd = (HWND)SDL_GetWindowHandle(window);

    //// Enabled layer
    //LONG style = GetWindowLong(hWnd, GWL_EXSTYLE);
    //SetWindowLong(hWnd, GWL_EXSTYLE, style | WS_EX_LAYERED);

    //return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
    return true;
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
        Vector2i WindowPos;
        Vector2i WindowSize;

        RECT WindowBounds;
        if (GetWindowRect(hWnd, &WindowBounds))
        {
            WindowPos.x = WindowBounds.left;
            WindowPos.y = WindowBounds.top;
            WindowSize.x = WindowBounds.right - WindowBounds.left;
            WindowSize.y = WindowBounds.bottom - WindowBounds.top;
        }

        AllWindows.emplace_back(windowTitle, WindowPos, WindowSize);
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
