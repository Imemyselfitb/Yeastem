#include "Yeastem.h"

#include "ProjectManager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <SDL3/SDL_dialog.h>
#include <string>

YEASTEM_BEGIN

std::string_view SectionView::getAttribute(std::string_view key) const
{
	for (KV attr : attributes)
		if (attr.key == key)
			return attr.value;

	return {};
}

const SectionView::KV_Multi* SectionView::getProperty(std::string_view key) const
{
	for (const KV_Multi& prop : properties)
		if (prop.key == key)
			return &prop;

	return nullptr;
}

const SectionView* INI_File::getSection(std::string_view name, uint32_t appearanceIdx)
{
	uint32_t currentAppearanceIdx = 0;
	for (const SectionView& section : sections)
	{
		if (section.header == name)
		{
			if (currentAppearanceIdx >= appearanceIdx)
				return &section;

			currentAppearanceIdx++;
		}
	}

	return nullptr;
}

std::string_view INI_Parser::Trim(std::string_view str)
{
	while (!str.empty() && isspace(str.front())) str.remove_prefix(1);
	while (!str.empty() && isspace(str.back())) str.remove_suffix(1);
	return str;
}

void INI_Parser::SplitPropertiesList(std::vector<std::string_view>& result, std::string_view s)
{
	s = Trim(s);
	while (!s.empty())
	{
		s = Trim(s);
		if (s.front() == '"')
		{
			s.remove_prefix(1);
			size_t endQuote = s.find('"');
			if (endQuote == std::string_view::npos)
				break;

			result.emplace_back(s.substr(0, endQuote));
			s.remove_prefix(endQuote + 1);
		}
		else
		{
			size_t comma = s.find(',');
			result.emplace_back(Trim(s.substr(0, comma)));
			if (comma == std::string_view::npos)
				break;

			s.remove_prefix(comma);
		}

		size_t comma = s.find(',');
		if (comma == std::string_view::npos)
			break;

		s.remove_prefix(comma + 1);
	}
}

void INI_Parser::Print(const INI_File& data)
{
	for (auto& section : data.sections)
	{
		YEASTEM_INFO("Section: " << section.header);
		for (auto& attr : section.attributes)
			YEASTEM_INFO("\t[[Attribute]] " << attr.key << " = " << attr.value);

		for (auto& prop : section.properties)
		{
			if (prop.values_count == 1)
			{
				YEASTEM_INFO("\t[[Property]] " << prop.key << " = " << section.allProperties[prop.values_start]);
			}
			else
			{
				std::cout << "\t[[Property]] " << prop.key << " = [";
				for (uint32_t i = 0; i < prop.values_count; i++)
				{
					if (i > 0)
						std::cout << ", ";

					std::cout << section.allProperties[prop.values_start + i];
				}
				std::cout << "]\n";
			}
		}
		YEASTEM_INFO("");
	}
}

void INI_Parser::ReadFile(INI_File& outData, const char* projectPath, const char* folderPath)
{
	if (!FileIO::checkIfExists(projectPath))
	{
		YEASTEM_ERROR("LOADING PROJECT: File " << projectPath << " does not exist!");
		return;
	}

	outData.file = FileIO::readFileText(projectPath);
	const char* ptr = outData.file.data();
	const char* end = outData.file.data() + outData.file.size();

	SectionView currentSection;

	while (ptr < end)
	{
		const char* lineStart = ptr;
		while (ptr < end && *ptr != '\n' && *ptr != '\r')
			ptr++;

		std::string_view line(lineStart, ptr - lineStart);
		line = Trim(line);

		if (line.empty() || line.front() == '#')
		{
			ptr++;
			continue;
		}

		if (line.front() == '[')
		{
			if (!currentSection.header.empty())
			{
				outData.sections.push_back(currentSection);
				currentSection = SectionView();
			}

			std::string_view inner = line.substr(1, line.size() - 2);
			inner = Trim(inner);

			size_t space = inner.find(' ');
			if (space == std::string_view::npos)
			{
				currentSection.header = inner;
			}
			else
			{
				currentSection.header = inner.substr(0, space);
				std::string_view attrs = inner.substr(space + 1);
				while (!attrs.empty())
				{
					size_t eq = attrs.find('=');
					if (eq == std::string_view::npos)
						break;
					
					std::string_view key = Trim(attrs.substr(0, eq));
					attrs.remove_prefix(eq + 1);

					std::string_view val;
					if (!attrs.empty() && attrs.front() == '"')
					{
						attrs.remove_prefix(1);
						size_t endQuote = attrs.find('"');
						if (endQuote == std::string_view::npos)
							break;

						val = attrs.substr(0, endQuote);
						attrs.remove_prefix(endQuote + 1);
					}
					else
					{
						size_t space = attrs.find(' ');
						val = Trim(attrs.substr(0, space));
						attrs = (space == std::string_view::npos) ? "" : attrs.substr(space + 1);
					}

					currentSection.attributes.push_back({ key, val });
					attrs = Trim(attrs);
				}
			}
		}
		else
		{
			auto eqPos = line.find('=');
			if (eqPos == std::string_view::npos)
				break;
			
			SectionView::KV_Multi entry;
			entry.key = Trim(line.substr(0, eqPos));
			entry.values_start = currentSection.allProperties.size();

			std::string_view values = Trim(line.substr(eqPos + 1));
			SplitPropertiesList(currentSection.allProperties, values);

			entry.values_count = currentSection.allProperties.size() - entry.values_start;
			currentSection.properties.push_back(entry);
		}
	}

	if (!currentSection.header.empty())
		outData.sections.push_back(currentSection);
}

void ProjectManager::LoadProject(const char* projectPath, const char* folderPath)
{
	m_HierarchyPanel.Init(Application::Get().GetCurrentScene());

	INI_File ProjectFile;
	INI_Parser::ReadFile(ProjectFile, projectPath, folderPath);

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

	std::string startScenePath = (std::string)applicationSection->allProperties[startScene->values_start];
	YEASTEM_INFO("Start Stem: " << startScenePath);
	LoadStem(
		(folderPath + (std::string)startScenePath).c_str(),
		folderPath, PROJECT_ROOT_NODE_NAME, 0
	);
}

struct StemComponent
{
	std::string Source;
};

void ProjectManager::LoadStem(const char* stemFilePath, const char* folderPath, const char* stemPath, HierarchyNode::NodeID stemID)
{
	std::cout << stemFilePath << "\n";

	Scene& currentScene = Application::Get().GetCurrentScene();

	INI_File StemFile;
	INI_Parser::ReadFile(StemFile, stemFilePath, folderPath);

	std::vector<std::string> stemPaths{ StemFile.sections.size() + 1 };
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
			Entity entity = currentScene.CreateEntity(entityPath.c_str(), entityName.c_str(), parent);
			HierarchyNode::NodeID entityHierarchyID = -1;
			m_HierarchyPanel.AddNode(entity, parentID, &entityHierarchyID);
			LoadEntity(m_HierarchyPanel.GetNode(entityHierarchyID), section, parentID, folderPath);
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
			Entity entity = currentScene.CreateEntity(entityPath.c_str(), entityName.c_str(), parent);

			const SectionView::KV_Multi* src = section.getProperty("src");
			YEASTEM_ASSERT(src, "LOADING STEM: External Stem has no source!");

			std::string relativePath = (std::string)section.allProperties[src->values_start];
			entity.AddComponent<StemComponent>(relativePath);
			m_HierarchyPanel.AddNode(entity, parentID);
			LoadStem((folderPath + relativePath).c_str(), folderPath, stemPaths[thisID].c_str(), parentID + thisID);
		}
		else if (section.header == "root")
		{
			Entity stem = m_HierarchyPanel.GetNode(stemID).entity;

			if (!stem.HasComponent<StemComponent>())
			{
				stem.AddComponent<StemComponent>(stemFilePath);

				TagComponent& tag = stem.GetComponent<TagComponent>();
				tag.Name = (std::string)section.getAttribute("name");
				tag.NodePath = stemPaths[0];
				stemPaths[0] = tag.NodePath + tag.Name + '/';
			}

			LoadEntity(m_HierarchyPanel.GetNode(stemID), section, -1, folderPath);
		}
	}
}

void ProjectManager::LoadEntity(HierarchyNode& node, const SectionView& section, int parentID, const char* folderPath)
{
	std::string_view type = section.getAttribute("type");
	if (type == "sprite")
	{
		LoadComponent<TransformComponent>(node.entity, section, folderPath);
		LoadComponent<RenderQuadComponent>(node.entity, section, folderPath);
		node.nodeType = NodeType::Sprite;
	}
	else if (type == "script")
	{
		LoadComponent<ScriptComponent>(node.entity, section, folderPath);
		node.nodeType = NodeType::Script;
	}
}

template<>
void ProjectManager::LoadComponent<TransformComponent>(Entity& entity, const SectionView& section, const char* folderPath)
{
	TransformComponent& transform = entity.AddComponent<TransformComponent>();

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
void ProjectManager::LoadComponent<RenderQuadComponent>(Entity& entity, const SectionView& section, const char* folderPath)
{
	RenderQuadComponent& renderQuad = entity.AddComponent<RenderQuadComponent>();

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
				folderPath + (std::string)section.allProperties[textures->values_start + i]
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
void ProjectManager::LoadComponent<ScriptComponent>(Entity& entity, const SectionView& section, const char* folderPath)
{
	ScriptComponent& script = entity.AddComponent<ScriptComponent>();

	const SectionView::KV_Multi* src = section.getProperty("src");
	if (src)
	{
		script.FilePath = folderPath + (std::string)section.allProperties[src->values_start];
	}
}

void ProjectManager::InitPanels()
{
	Application::Get().GetImGuiLayer().AddWindow([](bool& isOpen) {
		m_HierarchyPanel.Render(isOpen);
	}, true);
	Application::Get().GetImGuiLayer().AddWindow([](bool& isOpen) {
		PropertyPanel::Render(isOpen, m_HierarchyPanel);
	}, true);
	Application::Get().GetImGuiLayer().AddWindow([](bool& isOpen) {
		m_HierarchyPanel.nodeSelectPanel.Render(isOpen);
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([](bool& isOpen) {
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {
					std::cout << "New" << '\n';
				}
				if (ImGui::MenuItem("Open")) {
					std::cout << "Open" << '\n';
					
					//LoadProject();
				}
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
				if (ImGui::MenuItem("Quit")) { Application::Get().Close(); }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([](bool& isVisible) {
		Scene& currentScene = Application::Get().GetCurrentScene();
		EventHandler& eventHandler = currentScene.GetEventHandler();
		static ImVec2 prevSceneSize{ 0, 0 };

		ImGuiWindowFlags flags = 0;
		flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		//flags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("Stage", &isVisible, flags);

		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.y -= 35; // Menubar

		if (!currentScene.DoesFrameBufferExist())
			currentScene.CreateFrameBuffer((uint32_t)windowSize.x, (uint32_t)windowSize.y);

		uint32_t glColourAttachmentId = currentScene.GetFrameBufferColourAttachmentID();
		ImGui::Image((ImTextureID)(void*)(uintptr_t)glColourAttachmentId, windowSize);
		ImGui::End();

		if (windowSize != prevSceneSize)
		{
			currentScene.RecreateFrameBuffer((uint32_t)windowSize.x, (uint32_t)windowSize.y);
			Renderer::OnWindowResize();
			prevSceneSize = windowSize;
		}

		static bool wasRunningHeld = true, wasResetHeld = true;
		currentScene.IsRunning ^= eventHandler.IsKeyDown(SDLK_H) && !wasRunningHeld;
		currentScene.IsReset ^= eventHandler.IsKeyDown(SDLK_J) && !wasResetHeld;
		wasRunningHeld = eventHandler.IsKeyDown(SDLK_H);
		wasResetHeld = eventHandler.IsKeyDown(SDLK_J);

		currentScene.SceneSize = Vector2i{ windowSize.x, windowSize.y };
	}, false);

	Application::Get().GetImGuiLayer().AddWindow([&](bool& isOpen) {
		ShowStatisticsPanel(isOpen, -1);
	}, false);
}



YEASTEM_END
