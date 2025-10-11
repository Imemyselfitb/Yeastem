#pragma once	

#define IMGUI_DEBUG_PARANOID

#define APIENTRY
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

#include <utility>

#include <vector>
#include <unordered_map>

#include <memory>
#include <bitset>
#include <functional>

#include <iostream>

#if defined(YST_PLATFORM_WINDOWS)
#undef APIENTRY
#include <windows.h>
#undef max
#undef min
#endif

#include "Macros.h"