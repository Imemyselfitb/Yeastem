#include "Yeastem.h"

#include "ProjectManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <string>

YEASTEM_BEGIN

static ProjectManager s_ProjectManager;
ProjectManager& ProjectManager::Get()
{
	return s_ProjectManager;
}

void ProjectManager::CreateProject(const std::filesystem::path& projectPath)
{
	if (!s_CurrentProjectFolderPath.empty())
		UnloadProject();

	m_HierarchyPanel.Init();
	s_CurrentProjectPath = std::filesystem::absolute(projectPath);
	s_CurrentProjectFolderPath = s_CurrentProjectPath.parent_path();
	SaveProject();
}

void ProjectManager::LoadProject(const std::filesystem::path& projectPath)
{
	m_HierarchyPanel.Init();

	INI_File ProjectFile;
	INI_Parser::ReadFile(ProjectFile, projectPath);

	const SectionView* applicationSection = ProjectFile.getSection("Application");
	if (!applicationSection)
	{
		YEASTEM_ERROR("LOADING PROJECT: Application section not found!");
		return;
	}

#if YST_CONFIG_DEBUG
	const SectionView::KV_Multi* ProjectName = applicationSection->getProperty("name");
	if (ProjectName)
		YEASTEM_INFO("Loading Project: `" << applicationSection->allProperties[ProjectName->values_start] << "`");
#endif

	const SectionView::KV_Multi* startScene = applicationSection->getProperty("start_stem");
	if (!startScene)
	{
		YEASTEM_ERROR("LOADING PROJECT: Start Stem not found!");
		return;
	}

	const SectionView::KV_Multi* config = applicationSection->getProperty("config");
	if (!config)
	{
		YEASTEM_ERROR("LOADING PROJECT: Config not found!");
		return;
	}

	s_CurrentProjectPath = std::filesystem::absolute(projectPath);
	s_CurrentProjectFolderPath = s_CurrentProjectPath.parent_path();

	INI_File ConfigFile;
	std::filesystem::path projectConfigPath = applicationSection->allProperties[config->values_start].stringview;
	INI_Parser::ReadFile(ProjectFile, s_CurrentProjectFolderPath / projectConfigPath);

	const SectionView* editorSection = ProjectFile.getSection("Editor");
	if (!applicationSection)
	{
		YEASTEM_ERROR("LOADING PROJECT: Editor section not found!");
		return;
	}

	const SectionView::KV_Multi* currentOpenStem = editorSection->getProperty("current_scene");
	if (!currentOpenStem)
	{
		YEASTEM_ERROR("LOADING PROJECT: Current Scene section not found!");
		return;
	}
	m_Config.CurrentOpenStemFilePath = editorSection->allProperties[currentOpenStem->values_start].stringview;

	const SectionView::KV_Multi* allOpenStems = editorSection->getProperty("open_scenes");
	if (!allOpenStems)
	{
		YEASTEM_ERROR("LOADING PROJECT: Open Scenes section not found!");
		return;
	}

	ObjectID lastSceneID;
	bool openSceneFound = false;
	m_Config.OpenStemFilePaths.resize(allOpenStems->values_count);
	for (size_t i = 0; i < allOpenStems->values_count; i++)
	{
		std::filesystem::path stemPath = editorSection->allProperties[allOpenStems->values_start + i].stringview;
		m_Config.OpenStemFilePaths[i] = stemPath;
		ObjectID sceneID = LoadStem(s_CurrentProjectFolderPath / stemPath, PROJECT_ROOT_NODE_NAME);
		lastSceneID = sceneID;
		if (stemPath == m_Config.CurrentOpenStemFilePath)
		{
			Application::Get().SetCurrentScene(m_AllScenes.Get(sceneID));
			openSceneFound = true;
		}
	}
	if (!openSceneFound)
		Application::Get().SetCurrentScene(m_AllScenes.Get(lastSceneID));
}

ObjectID ProjectManager::LoadStem(
	const std::filesystem::path& stemFilePath, const std::string& stemPath, 
	HierarchyNode::NodeID stemID, ObjectID sceneID
) {
	if (stemID == -1)
	{
		sceneID = m_AllScenes.Load(std::template make_shared<Scene>(stemFilePath));
		Ref<Scene>& currentScene = m_AllScenes.Get(sceneID);
		currentScene->Init(sceneID, Application::Get().GetResourceManager());

		Entity rootEntity = currentScene->CreateEntity(stemFilePath.stem().string().c_str(), PROJECT_ROOT_NODE_NAME);
		stemID = m_HierarchyPanel.AddStem(rootEntity);
		currentScene->SetRootEntity(rootEntity);
	}

	Ref<Scene>& currentScene = m_AllScenes.Get(sceneID);
	
	INI_File StemFile;
	INI_Parser::ReadFile(StemFile, stemFilePath);

	std::vector<std::string> stemPaths{ StemFile.sections.size() + 1 }; // Path in engine editor tree
	stemPaths[0] = stemPath;
	
	for (SectionView& section : StemFile.sections)
	{
		if (section.header == "stem")
		{
			std::string entityName = (std::string)section.getAttribute("name");
			int parentID = std::stoi((std::string)section.getAttribute("parent"));
			std::string entityPath = stemPaths[parentID];
			int thisID = std::stoi((std::string)section.getAttribute("id"));
			stemPaths[thisID] = entityPath + entityName + '/';
			parentID += stemID;

			entt::entity parent = m_HierarchyPanel.GetNode(parentID).entity;
			Entity entity = currentScene->CreateEntity(entityPath.c_str(), entityName.c_str(), parent);
			HierarchyNode::NodeID entityHierarchyID = -1;
			m_HierarchyPanel.AddNode(entity, parentID, &entityHierarchyID);
			LoadEntity(m_HierarchyPanel.GetNode(entityHierarchyID), section, parentID);
		}
		else if (section.header == "extstem")
		{
			std::string entityName = (std::string)section.getAttribute("name");
			int parentID = std::stoi((std::string)section.getAttribute("parent"));
			std::string entityPath = stemPaths[parentID];
			int thisID = std::stoi((std::string)section.getAttribute("id"));
			stemPaths[thisID] = entityPath + entityName + '/';
			parentID += stemID;

			entt::entity parent = m_HierarchyPanel.GetNode(parentID).entity;
			Entity entity = currentScene->CreateEntity(entityPath.c_str(), entityName.c_str(), parent);

			const SectionView::KV_Multi* src = section.getProperty("src");
			if (!src)
			{
				YEASTEM_ERROR("LOADING STEM: External Stem has no source!");
				entity.Destroy();
				return sceneID;
			}

			std::string relativePath = (std::string)section.allProperties[src->values_start];
			entity.template AddComponent<ExtStemComponent>(relativePath);
			m_HierarchyPanel.AddNode(entity, parentID);
			LoadStem(s_CurrentProjectFolderPath / relativePath, stemPaths[thisID], parentID + thisID, sceneID);
		}
		else if (section.header == "root")
		{
			Entity stem = m_HierarchyPanel.GetNode(stemID).entity;

			if (stem.template HasComponent<ExtStemComponent>())
			{
				ExtStemComponent& stemComponent = stem.template GetComponent<ExtStemComponent>();
				stemComponent.Source = std::filesystem::absolute(stemFilePath);
			}
			else if (!stem.template HasComponent<StemComponent>())
			{
				StemComponent& stemComponent = stem.template AddComponent<StemComponent>(stemFilePath);
				stemComponent.nodeID = stemID;
				stemComponent.Source = std::filesystem::absolute(stemFilePath);
				stemComponent.SceneID = currentScene->GetID();

				TagComponent& tag = stem.template GetComponent<TagComponent>();
				tag.Name = (std::string)section.getAttribute("name");
				tag.NodePath = stemPaths[0];
				stemPaths[0] = tag.NodePath + tag.Name + '/';
			}

			LoadEntity(m_HierarchyPanel.GetNode(stemID), section, -1);
		}
	}

	return sceneID;
}

void ProjectManager::LoadEntity(
	HierarchyNode& node, const SectionView& section, int parentID
) {
	std::string_view type = section.getAttribute("type").stringview;
	if (type == "sprite")
	{
		LoadComponent<TransformComponent>(node.entity, section);
		LoadComponent<RenderQuadComponent>(node.entity, section);
		node.nodeType = NodeType::Sprite;
	}
	else if (type == "script")
	{
		LoadComponent<ScriptComponent>(node.entity, section);
		node.nodeType = NodeType::Script;
	}
}

template<>
void ProjectManager::LoadComponent<TransformComponent>(Entity& entity, const SectionView& section)
{
	TransformComponent& transform = entity.template AddComponent<TransformComponent>();

	const SectionView::KV_Multi* pos = section.getProperty("position");
	if (pos)
	{
		transform.Position = Vector2{
			std::stof(((std::string)section.allProperties[pos->values_start]).c_str()),
			std::stof(((std::string)section.allProperties[pos->values_start + 1]).c_str())
		};
	}

	const SectionView::KV_Multi* scale = section.getProperty("scale");
	if (scale)
	{
		transform.Scale = Vector2{
			std::stof(((std::string)section.allProperties[scale->values_start]).c_str()),
			std::stof(((std::string)section.allProperties[scale->values_start + 1]).c_str())
		};
	}

	const SectionView::KV_Multi* rotation = section.getProperty("rotation");
	if (rotation)
	{
		transform.Rotation = std::stof(((std::string)section.allProperties[rotation->values_start]).c_str());
	}
}

template<>
void ProjectManager::LoadComponent<RenderQuadComponent>(Entity& entity, const SectionView& section)
{
	RenderQuadComponent& renderQuad = entity.template AddComponent<RenderQuadComponent>();

	const SectionView::KV_Multi* current_texture = section.getProperty("current_texture");
	if (current_texture)
	{
		renderQuad.CurrentTexture = std::stoi(((std::string)section.allProperties[current_texture->values_start]).c_str());
	}

	const SectionView::KV_Multi* textures = section.getProperty("textures");
	if (textures)
	{
		renderQuad.Textures.reserve(textures->values_count);

		Yeastem::SingleResourceManager<Texture>& textureManager = Application::Get().GetResourceManager().Textures;
		for (size_t i = 0; i < textures->values_count; i++)
		{
			renderQuad.Textures.push_back(textureManager.Load(
				s_CurrentProjectFolderPath / section.allProperties[textures->values_start + i].stringview
			));
		}
	}

	const SectionView::KV_Multi* size = section.getProperty("size");
	if (size)
	{
		renderQuad.Size = Vector2{
			std::stof(((std::string)section.allProperties[size->values_start]).c_str()),
			std::stof(((std::string)section.allProperties[size->values_start + 1]).c_str())
		};
	}
}

template<>
void ProjectManager::LoadComponent<ScriptComponent>(Entity& entity, const SectionView& section)
{
	ScriptComponent& script = entity.template AddComponent<ScriptComponent>();

	const SectionView::KV_Multi* src = section.getProperty("src");
	if (src)
	{
		script.FilePath = s_CurrentProjectFolderPath / (std::string)section.allProperties[src->values_start];
	}
}

void ProjectManager::SaveProject()
{
	Ref<Scene>& currentScene = Application::Get().GetCurrentScene();
	entt::basic_view stemView = currentScene->GetRegistry().template view<StemComponent>();
	for (entt::entity entity : stemView)
	{
		std::cout << "Saving Stem: " << currentScene->GetRegistry().template get<TagComponent>(entity).Name << '\n';
		SaveStem(Entity{ entity, currentScene.get() });
	}
}

void ProjectManager::SaveStem(const Entity& entity)
{
	INI_File stemFile;
	StemComponent& stemComponent = entity.template GetComponent<StemComponent>();
	SaveNode(stemComponent.nodeID, stemFile, stemComponent.nodeID);
	stemFile.sections[0].header = "root";
	INI_Parser::WriteFile(stemFile, stemComponent.Source);
}

void ProjectManager::SaveNode(HierarchyNode::NodeID nodeID, INI_File& file, HierarchyNode::NodeID rootID) {
	SectionView& section = file.sections.emplace_back();
	HierarchyNode& node = m_HierarchyPanel.m_Hierarchy[nodeID];

	section.attributes.emplace_back("id", nodeID - rootID);
	if (node.ParentID != -1)
		section.attributes.emplace_back("parent", node.ParentID - rootID);

	if (node.entity.template HasComponent<ExtStemComponent>())
	{
		ExtStemComponent& extStemComponent = node.entity.template GetComponent<ExtStemComponent>();
		section.properties.push_back(SectionView::KV_Multi{ "src", section.allProperties.size(), 1 });
		section.allProperties.emplace_back(std::filesystem::relative(extStemComponent.Source, s_CurrentProjectFolderPath).string());
		section.header = "extstem";
		section.attributes.emplace_back("name", node.entity.template GetComponent<TagComponent>().Name);
		return;
	}
	
	section.header = "stem";
	SaveEntity(node, section);
	section.attributes.emplace_back("name", node.entity.template GetComponent<TagComponent>().Name);

	HierarchyNode::NodeID childID = node.FirstChildID;
	while (childID != -1)
	{
		SaveNode(childID, file, rootID);
		childID = m_HierarchyPanel.m_Hierarchy[childID].NextSiblingID;
	}
}

void ProjectManager::SaveEntity(const HierarchyNode& node, SectionView& section)
{
	if (node.nodeType == NodeType::Script)
	{
		section.attributes.emplace_back("type", "script");
		SaveComponent<ScriptComponent>(node.entity, section);
	}
	else if (node.nodeType == NodeType::Sprite)
	{
		section.attributes.emplace_back("type", "sprite");
		SaveComponent<TransformComponent>(node.entity, section);
		SaveComponent<RenderQuadComponent>(node.entity, section);
	}
	else
	{
		section.attributes.emplace_back("type", "node");
	}
}

template<>
void ProjectManager::SaveComponent<TransformComponent>(const Entity& entity, SectionView& section)
{
	TransformComponent& transform = entity.template GetComponent<TransformComponent>();
	section.properties.push_back(SectionView::KV_Multi{ "position", section.allProperties.size(), 2 });
	section.allProperties.emplace_back(transform.Position.x);
	section.allProperties.emplace_back(transform.Position.y);

	section.properties.push_back(SectionView::KV_Multi{ "scale", section.allProperties.size(), 2 });
	section.allProperties.emplace_back(transform.Scale.x);
	section.allProperties.emplace_back(transform.Scale.y);

	section.properties.push_back(SectionView::KV_Multi{ "rotation", section.allProperties.size(), 1 });
	section.allProperties.emplace_back(transform.Rotation);
}

template<>
void ProjectManager::SaveComponent<RenderQuadComponent>(const Entity& entity, SectionView& section)
{
	RenderQuadComponent& transform = entity.template GetComponent<RenderQuadComponent>();
	section.properties.push_back(SectionView::KV_Multi{ "size", section.allProperties.size(), 2 });
	section.allProperties.emplace_back(transform.Size.x);
	section.allProperties.emplace_back(transform.Size.y);

	section.properties.push_back(SectionView::KV_Multi{ "current_texture", section.allProperties.size(), 1 });
	section.allProperties.emplace_back(transform.CurrentTexture);

	section.properties.push_back(SectionView::KV_Multi{ "textures", section.allProperties.size(), transform.Textures.size() });
	for (size_t i = 0; i < transform.Textures.size(); i++)
	{
		Texture& texture = Application::Get().GetResourceManager().Textures.Get(transform.Textures[i]);
		std::filesystem::path relpath = std::filesystem::relative(std::filesystem::absolute(texture.getFilePath()), s_CurrentProjectFolderPath);
		section.allProperties.emplace_back(relpath.string());
	}
}

template<>
void ProjectManager::SaveComponent<ScriptComponent>(const Entity& entity, SectionView& section)
{
	ScriptComponent& script = entity.template GetComponent<ScriptComponent>();
	section.properties.push_back(SectionView::KV_Multi{ "src", section.allProperties.size(), 1 });
	std::filesystem::path relpath = std::filesystem::relative(std::filesystem::absolute(script.FilePath), s_CurrentProjectFolderPath);
	section.allProperties.emplace_back(relpath.string());
}

void ProjectManager::UnloadProject()
{
	m_HierarchyPanel.Clear();

	Application::Get().GetCurrentScene().reset(); // Reset the current scene to avoid dangling pointers
	m_AllScenes.Clear();
	s_CurrentProjectPath.clear();
	s_CurrentProjectFolderPath.clear();
}

void ProjectManager::ShowMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {
				std::cout << "New" << '\n';
				m_ProjectManagerPanel.Open(ProjectManagerPanel::State::ProjectCreation);
			}
			if (ImGui::MenuItem("Open")) {
				std::cout << "Open" << '\n';
				FileIO::FileSelect::open('PRJ');
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) { SaveProject(); }
			if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) { std::cout << "Save As" << '\n'; }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Cut", "Ctrl+X")) { s_ProjectManager.UnloadProject(); }
			if (ImGui::MenuItem("Copy", "Ctrl+C")) { std::cout << "Copy" << '\n'; }
			if (ImGui::MenuItem("Paste", "Ctrl+V")) { std::cout << "Paste" << '\n'; }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Quit")) { Application::Get().Close(); }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (FileIO::FileSelect::isSuccess && FileIO::FileSelect::selectedTag == 'PRJ')
	{
		UnloadProject();
		LoadProject(FileIO::FileSelect::selectedPaths[0]);
		FileIO::FileSelect::isSuccess = false;
	}
}

void ProjectManager::ShowStemBar()
{
	ImGuiWindowFlags sceneTabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeight() * 0.7f);
	ImGui::BeginTabBar("SceneTabBar", sceneTabBarFlags);

	for (const auto& [id, scene] : m_AllScenes.GetResources())
	{
		Entity entity = scene->GetRootEntity();
		StemComponent& stemComponent = entity.template GetComponent<StemComponent>();
		std::string name = entity.template GetComponent<TagComponent>().Name + "##" + std::to_string((uint64_t)id);

		bool open = true;
		if (ImGui::BeginTabItem(name.c_str(), &open))
		{
			m_CurrentStemRootNodeID = stemComponent.nodeID;
			if (Application::Get().GetCurrentScene()->GetID() != id)
				Application::Get().SetCurrentScene(m_AllScenes.Get(id));
			ImGui::EndTabItem();
		}

		if (!open)
			m_HierarchyPanel.DeleteStem(stemComponent.nodeID);
		else if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer >= 1.0f)
			ImGui::SetTooltip(stemComponent.Source.string().c_str());
	}
	ImGui::EndTabBar();
}

void ProjectManager::InitPanels()
{
	Application::Get().GetImGuiLayer().SetDockSpace([&](bool& isOpen) {
		ShowMenuBar();
		float menuBarHeight = ImGui::GetFrameHeight();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, menuBarHeight));
		ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, menuBarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags hostWindowFlags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

		ImGui::Begin("DockSpaceHost", nullptr, hostWindowFlags);

		ImGui::PopStyleVar(3);

		// Dockspace ID
		ImGuiID dockspaceID = ImGui::GetID("YeastemDockSpace");

		ShowStemBar();

		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		ImGui::End();
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		m_HierarchyPanel.Render(isOpen, m_CurrentStemRootNodeID);
	}, true);
	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		PropertyPanel::Render(isOpen, m_HierarchyPanel);
	}, true);
	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		m_HierarchyPanel.nodeSelectPanel.Render(isOpen);
	}, false);
	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		m_ProjectManagerPanel.Render(isOpen);
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([&](bool& isVisible) {
		Ref<Scene>& currentScene = Application::Get().GetCurrentScene();
		EventHandler& eventHandler = Application::Get().GetEventHandler();

		ImGuiWindowFlags flags = 0;
		flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		flags |= ImGuiWindowFlags_NoTitleBar;
		flags |= ImGuiWindowFlags_NoCollapse;
		flags |= ImGuiWindowFlags_NoScrollbar;
		//flags |= ImGuiWindowFlags_NoBackground;

		// Render Scene
		{
			YEASTEM_ScopedProfiler("SCENE: Render");
			currentScene->Render(Application::Get().GetResourceManager());
		}

		ImGui::Begin("Stage", &isVisible, flags);

		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.y -= 35; // Menubar

		if (!currentScene->DoesFrameBufferExist())
			currentScene->CreateFrameBuffer((uint32_t)windowSize.x, (uint32_t)windowSize.y);

		uint32_t glColourAttachmentId = currentScene->GetFrameBufferColourAttachmentID();
		ImGui::Image((ImTextureID)(void*)(uintptr_t)glColourAttachmentId, windowSize);
		ImGui::End();

		if (windowSize.x != currentScene->SceneSize.x || windowSize.y != currentScene->SceneSize.y)
		{
			currentScene->RecreateFrameBuffer((uint32_t)windowSize.x, (uint32_t)windowSize.y);
			Renderer::OnWindowResize();
		}

		static bool wasRunningHeld = true, wasResetHeld = true;
		currentScene->IsRunning ^= eventHandler.IsKeyDown(SDLK_H) && !wasRunningHeld;
		bool IsReset = eventHandler.IsKeyDown(SDLK_J) && !wasResetHeld;
		wasRunningHeld = eventHandler.IsKeyDown(SDLK_H);
		wasResetHeld = eventHandler.IsKeyDown(SDLK_J);

		if (IsReset)
			currentScene->ResetScripts();

		currentScene->SceneSize = Vector2i{ windowSize.x, windowSize.y };
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		ShowStatisticsPanel(isOpen, -1);
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		m_FileSystemPanel.Render(isOpen);
	}, true);
}



YEASTEM_END
