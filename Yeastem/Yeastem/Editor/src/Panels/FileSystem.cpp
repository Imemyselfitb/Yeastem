#include "FileSystem.h"

#include "ProjectManager.h"

#include <imgui.h>

YEASTEM_BEGIN

static struct Ignored_t {
	std::vector<const char*> RootFiles{};
	std::vector<const char*> RootFolders{ ".yeastem" };
	std::vector<const char*> Files{ ".yeast", "imgui.ini" };
	std::vector<const char*> Folders;
} s_Ignored;

static void RenderDir(const std::filesystem::path& dir, uint32_t depth = 0, bool isFile = false)
{
	if (!std::filesystem::exists(dir)) {
		ImGui::Text("Directory does not exist.");
		return;
	}

	ImGuiTreeNodeFlags defaultOpen = isFile ? ImGuiTreeNodeFlags_Leaf : (depth == 0) ? ImGuiTreeNodeFlags_DefaultOpen : 0;
	bool isOpen = ImGui::TreeNodeEx(dir.filename().string().c_str(), ImGuiTreeNodeFlags_OpenOnArrow | defaultOpen);

	if (ImGui::IsItemClicked())
	{
		// Handle file or directory selection here
	}

	if (isFile)
	{
		ImGui::TreePop();
		return;
	}
	else if (isOpen)
	{
		// Buffer files to render after directories
		std::vector<std::filesystem::path> files;

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dir))
		{
			const std::filesystem::path& path = entry.path();
			if (entry.is_directory())
			{
				bool ignored = false;
				std::string Foldername = std::filesystem::relative(path, dir).string();
				if (depth == 0) {
					for (const char* folders : s_Ignored.RootFolders)
					{
						if (Foldername.find(folders) != std::string::npos)
						{
							ignored = true;
							break;
						}
					}
				}
				for (const char* folders : s_Ignored.Folders)
				{
					if (ignored || Foldername.find(folders) != std::string::npos)
					{
						ignored = true;
						break;
					}
				}

				//printf("Folder name: %s\n", std::filesystem::relative(path, dir).string().c_str());
				if (!ignored)
					RenderDir(path, depth + 1);
			}
			else
			{
				bool ignored = false;
				std::string Filename = path.filename().string();
				if (depth == 0) {
					for (const char* files : s_Ignored.RootFiles)
					{
						if (Filename.find(files) != std::string::npos)
						{
							ignored = true;
							break;
						}
					}
				}
				for (const char* files : s_Ignored.Files)
				{
					if (ignored || Filename.find(files) != std::string::npos)
					{
						ignored = true;
						break;
					}
				}
				if (!ignored)
					files.push_back(path);
			}
		}

		for (const std::filesystem::path& file : files)
			RenderDir(file, depth + 1, true);

		ImGui::TreePop();
	}
}

void FileSystemPanel::Render(bool& isOpen)
{
	if (ImGui::Begin("File System", &isOpen))
		RenderDir(ProjectManager::ProjectFolderPath());
	
	ImGui::End();
}

YEASTEM_END
