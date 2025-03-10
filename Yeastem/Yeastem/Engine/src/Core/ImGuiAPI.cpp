#include "yst_pch.h"

#include "ImGuiAPI.h"
#include "Application.h"

YEASTEM_BEGIN

void ImGuiAPI::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui_ImplSDL2_InitForOpenGL(&m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsDark();
}

void ImGuiAPI::Update()
{
	for (ImGuiWindow& window : m_guiWindows)
	{
		window.Render();
	}
}

void ImGuiAPI::BackupFrame()
{
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void ImGuiAPI::AddWindow(Yeastem::ImGuiWindow::ShowFunction func)
{
	m_guiWindows.push_back(func);
}

void ImGuiAPI::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

YEASTEM_END
