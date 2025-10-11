#include "yst_pch.h"

#include "FileIO.h"

#include "Yeastem/Core/Application.h"	

YEASTEM_BEGIN

namespace FileIO
{
	std::string readFileText(const std::filesystem::path& path)
	{
		std::stringstream contents;
		std::fstream input(path, std::ios::in);
		contents << input.rdbuf();

		if (!contents.good())
		{
			YEASTEM_ERROR("Failed to Load File `" << path << "`!");
			return "";
		}

		return contents.str();
	}

	bool checkIfExists(const std::filesystem::path& path)
	{
		std::ifstream f(path.c_str());
		return f.good();
	}

	void FileSelect::open(const SDL_DialogFileFilter* filters, uint8_t filtersCount, uint64_t tag, bool allowMultiple)
	{
		isSuccess = false;
		isOpen = true;

		SDL_DialogFileCallback callback = [](void* userdata, const char* const* filelist, int filter) {
			if (!filelist || !filelist[0])
			{
				isOpen = false;
				return;
			}

			selectedPaths.clear();
			uint16_t pathCount = 0;
			for (const char* const* path = filelist; *path != nullptr; ++path)
				pathCount++;

			selectedPaths.resize(pathCount);
			for (uint16_t i = 0; i < pathCount; i++)
				selectedPaths[i] = std::filesystem::path(filelist[i]);

			selectedTag = (uint64_t)userdata;
			isSuccess = true;
			isOpen = false;
		};

		SDL_ShowOpenFileDialog(
			callback, (void*)tag, Application::Get().GetWindow(), 
			filters, filtersCount, nullptr, allowMultiple
		);
	}

	void FileSelect::open(uint64_t tag, bool allowMultiple)
	{
		SDL_DialogFileFilter filters[] = { {"All Files (*.*)", "*"} };
		open(filters, 1, tag, allowMultiple);
	}
}

YEASTEM_END
