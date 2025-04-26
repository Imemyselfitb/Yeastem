#include "Yeastem.h"

#include "NodeSelect.h"

#include <imgui.h>

YEASTEM_BEGIN

void NodeSelectPanel::Render(bool&)
{
	m_IsOpen &= m_SelectedType == m_SelectedTypePrevious;
	if (!m_IsOpen)
	{
		if (m_Callback)
		{
			m_Callback(m_SelectedType, m_SelectedTypePrevious);
			m_Callback = nullptr;
		}

		return;
	}

	ImGui::SetNextWindowSize(ImVec2(300, 200));
	if (ImGui::Begin("Select Node Type", &m_IsOpen))
	{
		RenderButton("Sprite", NodeType::Sprite);
		RenderButton("Script", NodeType::Script);
	}
	ImGui::End();
}

void NodeSelectPanel::RenderButton(const char* label, NodeType type)
{
	if (m_SelectedTypePrevious != type)
	{
		if (ImGui::Button(label))
			m_SelectedType = type;
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		if (ImGui::Button(label))
			m_SelectedType = type;
		ImGui::PopStyleColor();
	}
}

void NodeSelectPanel::ChangeType(Entity entity) const
{
	// Remove these
	uint32_t oldComponents = (~(uint32_t)m_SelectedType) & (uint32_t)m_SelectedTypePrevious;
	if (oldComponents)
		RemoveComponents(entity, oldComponents);

	// Add these
	uint32_t newComponents = (uint32_t)m_SelectedType & (~(uint32_t)m_SelectedTypePrevious);
	if (newComponents)
		AddComponents(entity, newComponents);
}

void NodeSelectPanel::RemoveComponents(Entity entity, uint32_t components) const
{
	if (components & ComponentTypes::Transform)
		entity.RemoveComponent<TransformComponent>();
	if (components & ComponentTypes::RenderQuad)
		entity.RemoveComponent<RenderQuadComponent>();
	if (components & ComponentTypes::Script)
		entity.RemoveComponent<ScriptComponent>();
}

void NodeSelectPanel::AddComponents(Entity entity, uint32_t components) const
{
	if (components & ComponentTypes::Transform)
		entity.AddComponent<TransformComponent>();
	if (components & ComponentTypes::RenderQuad)
		entity.AddComponent<RenderQuadComponent>();
	if (components & ComponentTypes::Script)
		entity.AddComponent<ScriptComponent>();
}

void NodeSelectPanel::OpenPanel(CallbackFn onSelected, NodeType defaultPrevious)
{
	m_IsOpen = true;
	m_Callback = onSelected;
	m_SelectedType = defaultPrevious;
	m_SelectedTypePrevious = defaultPrevious;
}

std::string NodeSelectPanel::GetNodeString(NodeType selected) const
{
	switch (selected)
	{
	case NodeType::Sprite: return "Sprite";
	case NodeType::Script: return "Script";
	case NodeType::None: return "Node";
	}

	YEASTEM_ASSERT("NodeSelectPanel::GetNodeString: Unknown Node Type!", false);
	return "None";
}

YEASTEM_END
