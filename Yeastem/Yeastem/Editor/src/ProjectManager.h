#pragma once

#include "Yeastem.h"

#include "Panels/SceneHierarchy.h"
#include "Panels/Statistics.h"
#include "Panels/Properties.h"

YEASTEM_BEGIN

struct SectionView
{
	struct KV
	{
		std::string_view key;
		std::string_view value;
	};

	struct KV_Multi
	{
		std::string_view key;
		size_t values_start{ 0 };
		size_t values_count{ 0 };
	};

	std::string_view header;
	std::vector<KV> attributes;

	std::vector<KV_Multi> properties; // Can have multiple values [list]
	std::vector<std::string_view> allProperties; // All values, stored in order of appearance

	std::string_view getAttribute(std::string_view key) const;
	const KV_Multi* getProperty(std::string_view key) const;
};

struct INI_File
{
	std::string file; // Will be referenced in SectionView
	std::vector<SectionView> sections;

	const SectionView* getSection(std::string_view name, uint32_t appearanceIdx = 0);
};

class INI_Parser
{
public:
	static void ReadFile(INI_File& outData, const char* projectPath, const char* folderPath);
	static void Print(const INI_File& data);

private:
	// Helper/Utils functions
	static std::string_view Trim(std::string_view str);
	static void SplitPropertiesList(std::vector<std::string_view>& result, std::string_view s);
};

class ProjectManager
{
public:
	static void InitPanels();

	static void LoadProject(const char* projectPath, const char* folderPath);
	static void LoadStem(const char* stemFilePath, const char* folderPath, const char* stemPath, HierarchyNode::NodeID stemID);
	static void LoadEntity(HierarchyNode& node, const SectionView& section, int parentID, const char* folderPath);

	template<typename Component> static void LoadComponent(Entity& entity, const SectionView& section, const char* folderPath)
	{ YEASTEM_ERROR("LOAD COMPONENT NOT IMPLEMENTED"); }
	template<> static void LoadComponent<TransformComponent>(Entity& entity, const SectionView& section, const char* folderPath);
	template<> static void LoadComponent<RenderQuadComponent>(Entity& entity, const SectionView& section, const char* folderPath);
	template<> static void LoadComponent<ScriptComponent>(Entity& entity, const SectionView& section, const char* folderPath);
	
	void SaveProject(const char* projectPath, const char* folderPath);

private:
	inline static HierarchyPanel m_HierarchyPanel;
};

constexpr const char* PROJECT_ROOT_NODE_NAME = "Tree/";

YEASTEM_END

