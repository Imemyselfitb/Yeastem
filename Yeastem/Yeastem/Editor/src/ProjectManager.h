#pragma once

#include "Yeastem.h"

#include "Panels/SceneHierarchy.h"
#include "Panels/Statistics.h"
#include "Panels/Properties.h"
#include "Panels/ProjectManagerPanel.h"
#include "Panels/FileSystem.h"

#include "Components.h"
#include "INIFile.h"

#include <filesystem>

YEASTEM_BEGIN

class ProjectManager
{
public:
	void InitPanels();

	static ProjectManager& Get();

	void LoadProject(const std::filesystem::path& projectPath);
	ObjectID LoadStem(
		const std::filesystem::path& stemFilePath, const std::string& stemPath, 
		HierarchyNode::NodeID stemID = -1, ObjectID sceneID = -1
	);

	static std::filesystem::path& ProjectPath() { return s_CurrentProjectPath; }
	static std::filesystem::path& ProjectFolderPath() { return s_CurrentProjectFolderPath; }

	void UnloadProject();

	void SaveProject();
	void SaveStem(const Entity& entity);

	void CreateProject(const std::filesystem::path& projectPath);

private:
	struct Config {
		std::filesystem::path CurrentOpenStemFilePath;
		std::vector<std::filesystem::path> OpenStemFilePaths;
	} m_Config;

private:
	void LoadEntity(HierarchyNode& node, const SectionView& section, int parentID);

	template<typename Component> void LoadComponent(Entity& entity, const SectionView& section)
	{ YEASTEM_ERROR("LOAD COMPONENT NOT IMPLEMENTED"); }
	template<> void LoadComponent<TransformComponent>(Entity& entity, const SectionView& section);
	template<> void LoadComponent<RenderQuadComponent>(Entity& entity, const SectionView& section);
	template<> void LoadComponent<ScriptComponent>(Entity& entity, const SectionView& section);
	
private:
	void SaveNode(HierarchyNode::NodeID nodeID, INI_File& file, HierarchyNode::NodeID rootID);
	void SaveEntity(const HierarchyNode& node, SectionView& section);
	
	template<typename Component> void SaveComponent(const Entity& entity, SectionView& section)
	{ YEASTEM_ERROR("LOAD COMPONENT NOT IMPLEMENTED"); }
	template<> void SaveComponent<TransformComponent>(const Entity& entity, SectionView& section);
	template<> void SaveComponent<RenderQuadComponent>(const Entity& entity, SectionView& section);
	template<> void SaveComponent<ScriptComponent>(const Entity& entity, SectionView& section);

	void ShowMenuBar();
	void ShowStemBar();

	HierarchyPanel& GetHierarchyPanel() { return m_HierarchyPanel; }

private:
	HierarchyPanel m_HierarchyPanel;
	FileSystemPanel m_FileSystemPanel;
	ProjectManagerPanel m_ProjectManagerPanel;
	HierarchyNode::NodeID m_CurrentStemRootNodeID = 0;

private:
	SingleResourceManager<Ref<Scene>> m_AllScenes;

private:
	inline static std::filesystem::path s_CurrentProjectPath;
	inline static std::filesystem::path s_CurrentProjectFolderPath;
};

constexpr const char* PROJECT_ROOT_NODE_NAME = "Tree/";

YEASTEM_END

