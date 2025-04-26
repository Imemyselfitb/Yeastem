#include "Yeastem.h"

#include "Properties.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

YEASTEM_BEGIN

void PropertyPanel::Render(bool& isOpen, HierarchyPanel& hierarchyPanel)
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	if (ImGui::Begin("Properties", &isOpen))
	{
		HierarchyNode::NodeID selectedNode = hierarchyPanel.GetFirstSelectedNode();
		if (selectedNode != -1)
		{
			Entity entity = hierarchyPanel.GetNode(selectedNode).entity;

			if (entity.HasComponent<TransformComponent>())
				RenderComponent<TransformComponent>(entity);
			if (entity.HasComponent<ScriptComponent>())
				RenderComponent<ScriptComponent>(entity);
			if (entity.HasComponent<RenderQuadComponent>())
				RenderComponent<RenderQuadComponent>(entity);
			if (entity.HasComponent<TagComponent>())
				RenderComponent<TagComponent>(entity);
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
		ImGui::DragInt("##x", &val, 1.0f, min, max);
	else
	{
		ImGui::BeginDisabled();
		ImGui::DragInt("##x", &val, 0.0f, min, max);
		ImGui::EndDisabled();
	}

	ImGui::PopID();
	ImGui::Columns(1);
}

static void RenderVector2f(const std::string& label, Vector2f& vec, float columnWidth)
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

	ImGui::DragFloat("##x", &vec.x);
	ImGui::NextColumn();

	// Property Name (next line)
	ImGui::NextColumn();

	ImGui::SetNextItemWidth(lineHeight);
	ImGui::TextColored(ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f }, "Y");
	ImGui::NextColumn();

	ImGui::DragFloat("##y", &vec.y);
	ImGui::PopID();

	ImGui::Columns(1);
}

template<> void PropertyPanel::RenderComponent<TransformComponent>(Entity& entity)
{
	TransformComponent& transform = entity.GetComponent<TransformComponent>();
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		RenderVector2f("Position", transform.Position, panelSize.x * 0.5f);
		RenderVector2f("Scale", transform.Scale, panelSize.x * 0.5f);

		float rotation = transform.Rotation * 180.0f / 3.14159265359f;
		RenderFloat("Rotation", rotation, panelSize.x * 0.5f);
		transform.Rotation = rotation / 180.0f * 3.14159265359f;

		float zLevel = transform.ZLevel * 100.0f;
		RenderFloat("Z Level", zLevel, panelSize.x * 0.5f);
		transform.ZLevel = zLevel / 100.0f;
	}
}

template<> void PropertyPanel::RenderComponent<RenderQuadComponent>(Entity& entity)
{
	SingleResourceManager<Texture>& textureManager = Application::Get().GetResourceManager().Textures;

	RenderQuadComponent& sprite = entity.GetComponent<RenderQuadComponent>();
	if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		RenderVector2f("Size", sprite.Size, panelSize.x * 0.5f);

		ImGui::PushID("Textures");
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, panelSize.x * 0.5f);
		ImGui::Text("Textures");
		ImGui::NextColumn();

		float padding = 5.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));

		for (ObjectID textureID : sprite.Textures)
		{
			Texture& currentTexture = textureManager.Get(textureID);
			bool showTextureInfo = ImGui::TreeNodeEx(currentTexture.getFileName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

			ImVec2 nodeRectMin = ImGui::GetItemRectMin();
			ImVec2 nodeRectMax = ImGui::GetItemRectMax();
			ImVec2 finalRectMin = nodeRectMin;
			ImVec2 finalRectMax = nodeRectMax;

			if (ImGui::IsItemClicked())
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					// TODO: MODIFY PATH
				}
				else
				{
					// TODO: SELECTED PROPERTIES
				}
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
	ScriptComponent& script = entity.GetComponent<ScriptComponent>();
	if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, panelSize.x * 0.5f);
		ImGui::Text("Path");
		ImGui::NextColumn();
		ImGui::Text(script.FilePath.c_str());
		ImGui::Columns(1);
	}
}

template<> void PropertyPanel::RenderComponent<TagComponent>(Entity& entity)
{
	TagComponent& tag = entity.GetComponent<TagComponent>();
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
