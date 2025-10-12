#include "Yeastem.h"

#include "Properties.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "ProjectManager.h"

YEASTEM_BEGIN

void PropertyPanel::Render(bool& isOpen, HierarchyPanel& hierarchyPanel)
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	if (ImGui::Begin("Properties", &isOpen))
	{
		SDL_SetWindowRelativeMouseMode(Application::Get().GetWindow(), false);

		HierarchyNode::NodeID selectedNode = hierarchyPanel.GetFirstSelectedNode();
		if (selectedNode != -1)
		{
			Entity entity = hierarchyPanel.GetNode(selectedNode).entity;

			if (entity.template HasComponent<TransformComponent>())
				RenderComponent<TransformComponent>(entity);
			if (entity.template HasComponent<ScriptComponent>())
				RenderComponent<ScriptComponent>(entity);
			if (entity.template HasComponent<RenderQuadComponent>())
				RenderComponent<RenderQuadComponent>(entity);
			if (entity.template HasComponent<TagComponent>())
				RenderComponent<TagComponent>(entity);
		}
		else
		{
			ImGui::Text("Select a Node to View its Properties");
		}
	}

	ImGui::End();
}

static void RenderFloat(const std::string& label, float& val, float columnWidth)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::DragFloat("##x", &val);
	ImGui::PopID();

	ImGui::Columns(1);
}

static void RenderInt(const std::string& label, int& val, float columnWidth, int min, int max)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	if (min != max)
	{
		ImGui::DragInt("##x", &val, 0.05f, min, max);
	}
	else
	{
		ImGui::BeginDisabled();
		ImGui::DragInt("##x", &val, 0.0f, min, max);
		if (ImGui::IsItemActive())
			SDL_SetWindowRelativeMouseMode(Application::Get().GetWindow(), true);
		ImGui::EndDisabled();
	}

	ImGui::PopID();
	ImGui::Columns(1);
}

static void RenderVector2f(const std::string& label, Vector2f& vec, float columnWidth, float speed = 1.0f)
{
	ImGui::PushID(label.c_str());

	ImGui::Columns(3, nullptr, false);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImGui::SetColumnWidth(1, lineHeight);
	ImGui::SetNextItemWidth(lineHeight);
	ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "X");
	ImGui::NextColumn();

	ImGui::DragFloat("##x", &vec.x, speed);
	ImGui::NextColumn();

	// Property Name (next line)
	ImGui::NextColumn();

	ImGui::SetNextItemWidth(lineHeight);
	ImGui::TextColored(ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f }, "Y");
	ImGui::NextColumn();

	ImGui::DragFloat("##y", &vec.y, speed);
	ImGui::PopID();

	ImGui::Columns(1);
}

template<> void PropertyPanel::RenderComponent<TransformComponent>(Entity& entity)
{
	TransformComponent& transform = entity.template GetComponent<TransformComponent>();
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		RenderVector2f("Position", transform.Position, panelSize.x * 0.5f);
		RenderVector2f("Scale", transform.Scale, panelSize.x * 0.5f, 0.1f);

		float rotation = transform.Rotation * 180.0f / 3.14159265359f;
		RenderFloat("Rotation", rotation, panelSize.x * 0.5f);
		transform.Rotation = rotation / 180.0f * 3.14159265359f;

		float zLevel = transform.ZLevel * 100.0f;
		RenderFloat("Z Level", zLevel, panelSize.x * 0.5f);
		transform.ZLevel = zLevel / 100.0f;
	}
}

static void OpenTextureFile(uint32_t texIndex, const char* currentPath = nullptr, bool createNew = false)
{
	SDL_DialogFileFilter textureOpenFileFilter[] = {
		{ "Image Files", "png;jpg;jpeg;bmp" },
		{ "PNG File (*.png)", "png" },
		{ "JPEG File (*.jpg,*.jpeg)", "jpg;jpeg" },
		{ "BMP File (*.bmp)", "bmp" },
	};
	uint64_t tag = ((uint64_t)'TX' << 48) + ((uint64_t)createNew << 32) + (uint64_t)texIndex;
	FileIO::FileSelect::open(textureOpenFileFilter, 4, tag, createNew);
}

template<> void PropertyPanel::RenderComponent<RenderQuadComponent>(Entity& entity)
{
	SingleResourceManager<Texture>& textureManager = Application::Get().GetResourceManager().Textures;

	RenderQuadComponent& sprite = entity.template GetComponent<RenderQuadComponent>();
	if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		RenderVector2f("Size", sprite.Size, panelSize.x * 0.5f, 0.1f);

		ImGui::PushID("Textures");
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, panelSize.x * 0.5f);
		ImGui::Text("Textures");
		ImGui::NextColumn();

		float padding = 5.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));

		for (uint32_t i = 0; i < sprite.Textures.size(); i++)
		{
			Texture& currentTexture = textureManager.Get(sprite.Textures[i]);

			std::filesystem::path textureRelPath = std::filesystem::relative(currentTexture.getFilePath(), ProjectManager::ProjectFolderPath());
			bool showTextureInfo = ImGui::TreeNodeEx(textureRelPath.string().c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

			ImVec2 nodeRectMin = ImGui::GetItemRectMin();
			ImVec2 nodeRectMax = ImGui::GetItemRectMax();
			ImVec2 finalRectMin = nodeRectMin;
			ImVec2 finalRectMax = nodeRectMax;

			if (ImGui::IsItemClicked())
			{
				if (ImGui::IsMouseDoubleClicked(0))
					OpenTextureFile(i, currentTexture.getFilePath().string().c_str());
			}
			else if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(currentTexture.getFilePath().string().c_str());
			}

			if (showTextureInfo)
			{
				float size = (panelSize.x * 0.5f) - 20.f; // 20px is from tree node
				ImVec2 boxSize = ImVec2(size, size + 10);

				ImVec2 imageSize = ImVec2((float)currentTexture.getWidth(), (float)currentTexture.getHeight());
				float scale = ImMin(boxSize.x / imageSize.x, boxSize.y / imageSize.y);
				imageSize *= scale;
				ImGui::Image((ImTextureID)(void*)(intptr_t)currentTexture.getOpenGlID(), imageSize);

				ImGui::TreePop();

				ImVec2 imageRectMin = ImGui::GetItemRectMin();
				ImVec2 imageRectMax = ImGui::GetItemRectMax();

				finalRectMin = ImVec2(fminf(finalRectMin.x, imageRectMin.x), fminf(finalRectMin.y, imageRectMin.y));
				finalRectMax = ImVec2(fmaxf(finalRectMax.x, imageRectMax.x), fmaxf(finalRectMax.y, imageRectMax.y));
			}

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRect(finalRectMin, finalRectMax, IM_COL32(255, 0, 0, 255), 5.0f);

			ImGui::NextColumn();
			ImGui::NextColumn();
		}

		// ADD NEW TEXTURE BUTTON
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			if (ImGui::Button("Add Texture", ImVec2(panelSize.x * 0.5f - padding * 2, 0)))
				OpenTextureFile((uint32_t)sprite.Textures.size()-1, nullptr, true);
			ImGui::PopStyleColor(3);
		}

		uint16_t tag = FileIO::FileSelect::selectedTag >> 48;
		if (FileIO::FileSelect::isSuccess && tag == 'TX')
		{
			FileIO::FileSelect::isSuccess = false; // Reset success flag

			uint32_t textureIdx = FileIO::FileSelect::selectedTag & 0xFFFFFFFF;
			bool createNew = FileIO::FileSelect::selectedTag & 0x100000000;
			SingleResourceManager<Texture>& textureManager = Application::Get().GetResourceManager().Textures;

			for (const std::filesystem::path& file : FileIO::FileSelect::selectedPaths)
			{
				if (std::filesystem::exists(file))
				{
					if (createNew)
						sprite.Textures.push_back(textureManager.Load(file));
					else
						sprite.Textures[textureIdx] = textureManager.Load(file);
				}
			}
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();

		int currentTexture = sprite.CurrentTexture;
		RenderInt("Current Texture", currentTexture, panelSize.x * 0.5f, 0, (int)sprite.Textures.size()-1);
		sprite.CurrentTexture = currentTexture;
	}
}

template<> void PropertyPanel::RenderComponent<ScriptComponent>(Entity& entity)
{
	ScriptComponent& script = entity.template GetComponent<ScriptComponent>();
	if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, panelSize.x * 0.5f);
		ImGui::Text("Path");
		ImGui::NextColumn();
		std::filesystem::path textureRelPath = std::filesystem::relative(script.FilePath, ProjectManager::ProjectFolderPath());
		ImGui::Text(textureRelPath.string().c_str());
		if (ImGui::IsItemClicked())
		{
			if (ImGui::IsMouseDoubleClicked(0))
			{
				SDL_DialogFileFilter textureOpenFileFilter[] = {
					{ "Lua File (*.lua)", "lua" },
				};
				FileIO::FileSelect::open(textureOpenFileFilter, 1, 'LUA');
			}
		}
		ImGui::Columns(1);

		if (FileIO::FileSelect::isSuccess && (FileIO::FileSelect::selectedTag == 'LUA'))
		{
			FileIO::FileSelect::isSuccess = false; // Reset success flag

			const std::filesystem::path& file = FileIO::FileSelect::selectedPaths[0];
			if (std::filesystem::exists(file))
			{
				script.FilePath = file;
			}
		}
	}
}

template<> void PropertyPanel::RenderComponent<TagComponent>(Entity& entity)
{
	TagComponent& tag = entity.template GetComponent<TagComponent>();
	if (ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, panelSize.x * 0.5f);
		ImGui::Text("Node Path");
		ImGui::NextColumn();
		ImGui::Text(tag.NodePath.c_str());
		ImGui::Columns(1);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, panelSize.x * 0.5f);
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text(tag.Name.c_str());
		ImGui::Columns(1);
	}
}

YEASTEM_END
