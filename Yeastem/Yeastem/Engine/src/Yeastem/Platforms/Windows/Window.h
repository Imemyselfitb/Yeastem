#pragma once

YEASTEM_BEGIN

bool MakeWindowTransparent(SDL_Window* window, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void GetWindowData();

struct Windows
{
    inline Windows() {}
    inline Windows(const std::string& title, Vector2i position, Vector2i size)
        :Title(title), Position(position), Size(size)
    {
    }

    std::string Title;
    Vector2i Position;
    Vector2i Size;
};

inline std::vector<Windows> AllWindows;

YEASTEM_END
