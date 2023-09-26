#include "raylib.h"

int main(int argc, char* argv[])
{
#define Colour Color

	int width = 640;
	int height = 480;
	Raylib_InitWindow(width, height, "Yeastem Runtime");

	Raylib_SetTargetFPS(60);
	Colour m_BackgroundCol { 0, 255, 255, 255 };

	while (!Raylib_WindowShouldClose())    // Detect window close button or ESC key
	{
		Raylib_BeginDrawing();
		Raylib_ClearBackground(m_BackgroundCol);
		Raylib_DrawText("RUNTIME text!!!!", 190, 200, 20, LIGHTGRAY);
		Raylib_EndDrawing();
	}

	Raylib_CloseWindow();
}