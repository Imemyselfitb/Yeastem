#include "Yeastem.h"

#include "ProjectManagerPanel.h"

#include <imgui.h>
#include <SDL3/SDL_dialog.h>

#include "ProjectManager.h"

YEASTEM_BEGIN

static struct ProjectCreationData
{
	char projectName[64] = "New Project";
	char projectFolderName[64] = "new-project";
	char projectPath[128] = "C:";
	bool isSyncing = true;
	bool isChanged = false;
} s_ProjectCreationData;

static bool Toggle(const char* label, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.5f;

	ImGui::PushID("Toggle");
	ImGui::InvisibleButton(label, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;
	ImGui::PopID();

	float t = *v ? 1.0f : 0.0f;
	ImU32 bg_color = ImGui::GetColorU32(*v ? ImGuiCol_ButtonActive : ImGuiCol_Button);

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), bg_color, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - height), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	
	ImGui::SameLine();
	ImGui::Text(label);
	
	return *v;
}

void ProjectManagerPanel::Render(bool&)
{
	if (!m_IsOpen)
	{
		if (s_ProjectCreationData.isChanged)
			s_ProjectCreationData = ProjectCreationData{};

		return;
	}

	if (m_ProjectState == State::ProjectCreation)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiTreeNodeFlags_DefaultOpen);
		if (ImGui::Begin("New Project", &m_IsOpen))
		{
			std::filesystem::path path(s_ProjectCreationData.projectPath);
			path = path / s_ProjectCreationData.projectFolderName / "project.yeast";

			ImGuiInputTextCallback stopTextSync = [](ImGuiInputTextCallbackData* data) -> int {
				if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
				{
					s_ProjectCreationData.isChanged = true;
					s_ProjectCreationData.isSyncing = false;
				}
				return 0;
			};

			ImGuiInputTextCallback syncText = [](ImGuiInputTextCallbackData* data) -> int {
				if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
				{
					for (int i = 0; i < data->BufTextLen; i++)
					{
						if (std::isdigit(data->Buf[i]))
							s_ProjectCreationData.projectFolderName[i] = data->Buf[i];
						else if (std::isalpha(data->Buf[i]))
							s_ProjectCreationData.projectFolderName[i] = std::tolower(data->Buf[i]);
						else
							s_ProjectCreationData.projectFolderName[i] = '-';
					}
					s_ProjectCreationData.projectFolderName[data->BufTextLen] = '\0';
				}
				return 0;
			};

			ImGui::Text("Project Name");
			ImGui::SameLine();
			ImGui::InputText(
				"##ProjectName", s_ProjectCreationData.projectName, IM_ARRAYSIZE(s_ProjectCreationData.projectName),
				ImGuiInputTextFlags_CallbackEdit, syncText
			);
			ImGui::SameLine();
			Toggle("SYNC", &s_ProjectCreationData.isSyncing);

			ImGui::Text("Project Path");
			ImGui::SameLine();
			if (ImGui::Button("SelectFolder"))
			{
				SDL_DialogFileCallback callback = [](void* userdata, const char* const* filelist, int filter) {
					if (!filelist || !filelist[0])
						return;

					std::filesystem::path dir = std::filesystem::path(filelist[0]).parent_path();
					if (!std::filesystem::exists(dir.parent_path()))
						return;

					std::string folderName = dir.string();
					memcpy(s_ProjectCreationData.projectFolderName, folderName.c_str(), folderName.size());
					s_ProjectCreationData.projectFolderName[folderName.size()] = '\0';

					std::string folder = dir.parent_path().string();
					memcpy(s_ProjectCreationData.projectPath, folder.c_str(), folder.size());
					s_ProjectCreationData.projectPath[folder.size()] = '\0';
				};

				SDL_DialogFileFilter filter[] = { {"Yeastem Project", "yeast"} };
				SDL_ShowSaveFileDialog(callback, nullptr, Application::Get().GetWindow(), filter, 1, path.string().c_str());
			}
			ImGui::SameLine();
			std::string fullPath = path.string();
			ImGui::InputText(
				"##ProjectPath", &fullPath[0], fullPath.size()+1,
				ImGuiInputTextFlags_CallbackEdit, stopTextSync
			);

			if (ImGui::Button("Create"))
			{
				std::cout << "Creating project at: " << path.string() << '\n';
				ProjectManager::Get().CreateProject(path);

				m_IsOpen = false;
				s_ProjectCreationData.isChanged = false;
			}
		}
		ImGui::End();
	}
}

void ProjectManagerPanel::Open(State state)
{
	m_ProjectState = state;
	m_IsOpen = true;
}

YEASTEM_END
