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

	ImGui_ImplSDL2_InitForOpenGL(&this->m_window, this->m_context);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsDark();
}

ImGuiAPI::ImGuiAPI(SDL_Window* window, SDL_GLContext* context)
	:m_window(*window), m_context(*context)
{
	this->m_guiWindows.emplace_back([](bool&) {
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {
					std::cout << "New" << '\n';
				}
				if (ImGui::MenuItem("Open")) { std::cout << "Open" << '\n'; }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { std::cout << "Save" << '\n'; }
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) { std::cout << "Save As" << '\n'; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Cut", "Ctrl+X")) { std::cout << "Cut" << '\n'; }
				if (ImGui::MenuItem("Copy", "Ctrl+C")) { std::cout << "Copy" << '\n'; }
				if (ImGui::MenuItem("Paste", "Ctrl+V")) { std::cout << "Paste" << '\n'; }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Quit")) { Application::s_IsRunning = false; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	});
}



void ImGuiAPI::Update()
{
	ImGuiNextFrame();

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
	this->m_guiWindows.push_back(func);
}

void ImGuiAPI::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

YEASTEM_END
