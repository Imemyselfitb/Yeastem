#pragma once

YEASTEM_BEGIN

namespace FileIO
{
	std::string readFileText(const std::filesystem::path& path);
	bool checkIfExists(const std::filesystem::path& path);

	class FileSelect
	{
	public:
		static void open(const SDL_DialogFileFilter* filters, uint8_t filtersCount, uint64_t tag = 0, bool allowMultiple = false);
		static void open(uint64_t tag = 0, bool allowMultiple = false);

		inline static bool isOpen = false;
		inline static bool isSuccess = false;
		inline static uint64_t selectedTag = 0;
		inline static std::vector<std::filesystem::path> selectedPaths;

	public:
		inline static const SDL_DialogFileFilter DEFAULT_FILTER = { "All Files (*.*)", "*" };
	};
}

YEASTEM_END
