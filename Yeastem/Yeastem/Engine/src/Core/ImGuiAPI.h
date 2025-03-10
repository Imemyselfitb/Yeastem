#pragma once

#include "ImGuiWindow.h"

YEASTEM_BEGIN

class ImGuiAPI
{
public:
	ImGuiAPI(SDL_Window* window, SDL_GLContext* context) :m_window(*window), m_context(*context) {}
	~ImGuiAPI() {}

public:
	void Init();
	void Update();
	void BackupFrame();
	void AddWindow(Yeastem::ImGuiWindow::ShowFunction);
	void Destroy();

public:
	SDL_Window& m_window;
	SDL_GLContext& m_context;

private:
	std::vector<ImGuiWindow> m_guiWindows;
};

#define ImGuiNextFrame() ImGui_ImplOpenGL3_NewFrame(); \
			ImGui_ImplSDL2_NewFrame(); \
			ImGui::NewFrame()

YEASTEM_END
