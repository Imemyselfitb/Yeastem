#pragma once

#include "ImGuiWindow.h"

YEASTEM_BEGIN

class ImGuiAPI
{
public:
	ImGuiAPI() {}
	~ImGuiAPI() {}

public:
	void Init(SDL_Window* window, SDL_GLContext& context);
	void Update();
	void BackupFrame();
	void AddWindow(ImGuiWindow::ShowFunction function, bool canClose);
	void Destroy();

	void SetDockSpace(ImGuiWindow::ShowFunction function, bool canClose) { m_DockSpaceWindow = { function, canClose }; }
	void UpdateDockSpace() { m_DockSpaceWindow.Render(); }

private:
	ImGuiWindow m_DockSpaceWindow;
	std::vector<ImGuiWindow> m_GuiWindows;
};

#define ImGuiNextFrame() ImGui_ImplOpenGL3_NewFrame(); \
			ImGui_ImplSDL3_NewFrame(); \
			ImGui::NewFrame()

YEASTEM_END
