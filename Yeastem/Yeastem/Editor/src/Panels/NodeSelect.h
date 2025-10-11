#pragma once

YEASTEM_BEGIN

enum class NodeType : uint32_t
{
	None = 0,
	Sprite = ComponentTypes::Transform | ComponentTypes::RenderQuad,
	Script = ComponentTypes::Script,
};

class NodeSelectPanel
{
public:
	NodeSelectPanel() = default;

public:
	using CallbackFn = std::function<void(NodeType selected, NodeType previous)>;

public:
	void Render(bool&);
	void OpenPanel(CallbackFn onSelected, NodeType defaultPrevious = NodeType::None);

	std::string GetNodeString(NodeType selected) const;

	void ChangeType(Entity entity) const;
	void ChangeType(Entity entity, NodeType from, NodeType to) const;
	void RemoveComponents(Entity entity, uint32_t components) const;
	void AddComponents(Entity entity, uint32_t components) const;

private:
	void RenderButton(const char* label, NodeType type);

private:
	NodeType m_SelectedType = NodeType::None;
	NodeType m_SelectedTypePrevious = NodeType::None;
	CallbackFn m_Callback = nullptr;
	bool m_IsOpen = false;
};

YEASTEM_END
