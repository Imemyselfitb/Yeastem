#include "Yeastem.h"

#include "SceneHierarchy.h"

#include "Components.h"

#include <imgui.h>
#include <string>

YEASTEM_BEGIN

void HierarchyPanel::Init()
{
	m_SelectedNodes.resize(m_MinSelectedNodeCount, SelectedNode{ InvalidID, true });
	m_SelectedNodesCount = 0;
}

void HierarchyPanel::ClearSelectedNodes()
{
	for (uint32_t i = 0; i < m_SelectedNodes.size(); i++)
	{
		if (m_SelectedNodes[i].IsTombstone == false)
		{
			m_Hierarchy[m_SelectedNodes[i].ID].IsSelected = false;
			m_SelectedNodes[i].IsTombstone = true;
		}
	}

	if (m_TombstoneSelectedNodeCount > m_MinSelectedNodeCount)
		m_SelectedNodes.erase(m_SelectedNodes.begin() + m_MinSelectedNodeCount, m_SelectedNodes.end());

	m_TombstoneSelectedNodeCount = m_MinSelectedNodeCount;
	m_SelectedNodesCount = 0;
}

void HierarchyPanel::AddSelectedNode(HierarchyNode::NodeID nodeID)
{
	if (m_Hierarchy[nodeID].IsSelected)
		return;

	if (m_TombstoneSelectedNodeCount < 1)
		m_SelectedNodes.emplace_back(nodeID);
	else
	{
		for (uint32_t i = 0; i < m_SelectedNodes.size(); i++)
		{
			if (m_SelectedNodes[i].IsTombstone == true)
			{
				m_SelectedNodes[i].ID = nodeID;
				m_SelectedNodes[i].IsTombstone = false;
				m_TombstoneSelectedNodeCount--;
				break;
			}
		}
	}

	m_Hierarchy[nodeID].IsSelected = true;
	m_SelectedNodesCount++;
}

void HierarchyPanel::AddSelectedNodeChildren(HierarchyNode::NodeID nodeID)
{
	if (nodeID == InvalidID)
		return;

	AddSelectedNode(nodeID);

	HierarchyNode::NodeID childID = m_Hierarchy[nodeID].FirstChildID;
	while (childID != InvalidID)
	{
		AddSelectedNodeChildren(childID);
		childID = m_Hierarchy[childID].NextSiblingID;
	}
}

void HierarchyPanel::AddSelectedNodeRange(HierarchyNode::NodeID startID, HierarchyNode::NodeID endID)
{
	HierarchyNode::NodeID parentA = startID;
	uint32_t depthA = 0;
	std::vector<HierarchyNode::NodeID> hierarchyA;
	while (parentA != InvalidID)
	{
		hierarchyA.push_back(parentA);
		parentA = m_Hierarchy[parentA].ParentID;
	}

	HierarchyNode::NodeID parentB = endID;
	uint32_t depthB = 0;
	std::vector<HierarchyNode::NodeID> hierarchyB;
	while (parentB != InvalidID)
	{
		hierarchyB.push_back(parentB);
		parentB = m_Hierarchy[parentB].ParentID;
	}

	uint32_t i = (uint32_t)hierarchyA.size() - 1;
	uint32_t j = (uint32_t)hierarchyB.size() - 1;
	bool aFoundFirst = i == 0; // Default Value, if either of Selected nodes is root

	if (i != 0 && j != 0)
	{
		while (i > 0 && j > 0 && hierarchyA[i--] == hierarchyB[j--]) {}

		bool eitherFound = false;
		HierarchyNode::NodeID childID = m_Hierarchy[hierarchyA[i + 1]].FirstChildID;
		while (childID != InvalidID)
		{
			if (childID == hierarchyA[i] || childID == hierarchyB[j])
			{
				if (eitherFound == true)
					break; // Both found

				eitherFound = true;
				aFoundFirst = (childID == hierarchyA[i]);
				AddSelectedNode(childID);

				// Select Children (skip node itsef - already selected)
				for (uint32_t k = 1; k < i; k++)
				{
					HierarchyNode::NodeID nodeID = aFoundFirst ? hierarchyA[k] : hierarchyB[k];
					while (nodeID != InvalidID)
					{
						AddSelectedNode(nodeID);
						nodeID = m_Hierarchy[nodeID].NextSiblingID;
					}
				}
			}
			else if (eitherFound)
				AddSelectedNodeChildren(childID);

			childID = m_Hierarchy[childID].NextSiblingID;
		}
	}

	for (uint32_t k = 0; k < i; k++)
	{
		HierarchyNode::NodeID nodeID = aFoundFirst ? hierarchyA[k] : hierarchyB[k];
		HierarchyNode::NodeID parentID = m_Hierarchy[nodeID].ParentID;
		HierarchyNode::NodeID siblingID = m_Hierarchy[parentID].FirstChildID;
		while (siblingID != InvalidID)
		{
			if (siblingID == nodeID)
				break; // Break out of while loop

			AddSelectedNode(siblingID);
			siblingID = m_Hierarchy[siblingID].NextSiblingID;
		}
	}
	AddSelectedNode(endID);
}

void HierarchyPanel::RemoveSelectedNode(HierarchyNode::NodeID nodeID)
{
	for (uint32_t i = 0; i < m_SelectedNodes.size(); i++)
	{
		if (m_SelectedNodes[i].IsTombstone == false && m_SelectedNodes[i].ID == nodeID)
		{
			m_Hierarchy[nodeID].IsSelected = false;
			m_SelectedNodes[i].IsTombstone = true;
			m_TombstoneSelectedNodeCount++;
			m_SelectedNodesCount--;

			if (m_LastSelectedNodeID == nodeID)
				m_LastSelectedNodeID = InvalidID;
		}
	}
}

bool HierarchyPanel::IsNodeSelected(HierarchyNode::NodeID nodeID)
{
	for (uint32_t i = 0; i < m_SelectedNodes.size(); i++)
	{
		if (m_SelectedNodes[i].IsTombstone == false && m_SelectedNodes[i].ID == nodeID)
			return true;
	}

	return false;
}

void HierarchyPanel::Render(bool& isOpened, HierarchyNode::NodeID nodeID)
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	if (ImGui::Begin("Scene Hierarchy", &isOpened))
	{
		if (nodeID < m_Hierarchy.size() && !m_Hierarchy[nodeID].IsTombstone)
			RenderNode(nodeID);
	}
	ImGui::End();
}

static char RenameBuffer[256];

void HierarchyPanel::OnNodeClicked(HierarchyNode::NodeID nodeID, const char* nodeName)
{
	if (m_SelectedNodesCount > 0)
	{
		if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
		{
			AddSelectedNodeRange(m_LastSelectedNodeID, nodeID);
			return;
		}
		else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			if (IsNodeSelected(nodeID))
				RemoveSelectedNode(nodeID);
			else
			{
				AddSelectedNode(nodeID);
				m_LastSelectedNodeID = nodeID;
			}
			return;
		}
	}

	ClearSelectedNodes();
	AddSelectedNode(nodeID);
	m_LastSelectedNodeID = nodeID;

	if (ImGui::IsMouseDoubleClicked(0))
	{
		m_RenamingNodeID = nodeID;
		strncpy_s(RenameBuffer, nodeName, sizeof(RenameBuffer) - 1);
		RenameBuffer[sizeof(RenameBuffer) - 1] = '\0';
	}
}

bool HierarchyPanel::RenderNode(HierarchyNode::NodeID nodeID)
{
	HierarchyNode& node = m_Hierarchy[nodeID];
	TagComponent& tag = node.entity.template GetComponent<TagComponent>();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= (node.IsSelected) * ImGuiTreeNodeFlags_Selected;
	flags |= (node.FirstChildID == InvalidID) * ImGuiTreeNodeFlags_Leaf;

	ImGui::PushID(nodeID);

	// Show Tree (Empty Name -> this will be filled in later, to allow renaming)
	ImGui::SetNextItemOpen(node.IsExpanded);
	node.IsExpanded = ImGui::TreeNodeEx((void*)(intptr_t)(entt::entity)node.entity, flags, "");

	if (ImGui::IsItemClicked())
		OnNodeClicked(nodeID, tag.Name.c_str());

	bool treeModified = false;

	if (nodeID == m_RenamingNodeID)
	{
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150);
		ImGui::SetKeyboardFocusHere();
		ImGuiInputTextFlags renameFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank;
		if (ImGui::InputText("##rename", RenameBuffer, sizeof(RenameBuffer), renameFlags))
		{
			std::string name = RenameBuffer;
			AssignChildName(m_Hierarchy[node.ParentID], nodeID, name);
			RenameNode(node, name);
			m_RenamingNodeID = InvalidID;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			m_RenamingNodeID = InvalidID;
	}
	else
	{
		// Show Node Name
		ImGui::SameLine();
		if (ImGui::Selectable(tag.Name.c_str(), node.IsSelected, ImGuiSelectableFlags_SpanAllColumns))
			OnNodeClicked(nodeID, tag.Name.c_str());
		else if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			OnNodeClicked(nodeID, tag.Name.c_str());

		// Moving Nodes
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("DRAG_AND_DROP_NODE", &nodeID, sizeof(HierarchyNode::NodeID));

			//ImGui::Text("%s", tag.Name.c_str());
			for (int i = 0; i < m_SelectedNodes.size(); i++)
			{
				if (m_SelectedNodes[i].IsTombstone == false)
				{
					TagComponent& selectedTag = m_Hierarchy[m_SelectedNodes[i].ID].entity.template GetComponent<TagComponent>();
					ImGui::Text("%s", selectedTag.Name.c_str());
				}
			}
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_AND_DROP_NODE");
			if (payload)
			{
				for (int i = 0; i < m_SelectedNodes.size(); i++)
				{
					if (m_SelectedNodes[i].IsTombstone == false)
					{
						MoveNode(m_SelectedNodes[i].ID, nodeID);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Right click menu
		if (ImGui::BeginPopupContextItem())
		{
			ClearSelectedNodes();
			AddSelectedNode(nodeID);
			m_LastSelectedNodeID = nodeID;

			if (ImGui::MenuItem("Add Node"))
			{
				nodeSelectPanel.OpenPanel([this](NodeType selected, NodeType previous) {
					if (selected == NodeType::None)
						return;

					TagComponent& NodeParentTag = m_Hierarchy[m_LastSelectedNodeID].entity.template GetComponent<TagComponent>();

					Ref<Scene>& currentScene = Application::Get().GetCurrentScene();
					Entity newEntity = currentScene->CreateEntity(
						(NodeParentTag.NodePath + NodeParentTag.Name + '/').c_str(),
						nodeSelectPanel.GetNodeString(selected).c_str(), m_Hierarchy[m_LastSelectedNodeID].entity
					);

					HierarchyNode::NodeID newEntityID = InvalidID;
					AddNode(newEntity, m_LastSelectedNodeID, &newEntityID);

					nodeSelectPanel.ChangeType(newEntity, m_Hierarchy[newEntityID].nodeType, selected);

					ClearSelectedNodes();
					AddSelectedNode(newEntityID);

					m_RenamingNodeID = newEntityID;
					TagComponent& newTag = newEntity.template GetComponent<TagComponent>();
					strncpy_s(RenameBuffer, newTag.Name.c_str(), sizeof(RenameBuffer) - 1);
					RenameBuffer[sizeof(RenameBuffer) - 1] = '\0';
				});
			}
			if (ImGui::MenuItem("Change Type"))
			{
				nodeSelectPanel.OpenPanel([this](NodeType selected, NodeType previous) {
					if (selected != previous)
					{
						nodeSelectPanel.ChangeType(m_Hierarchy[m_LastSelectedNodeID].entity);
						m_Hierarchy[m_LastSelectedNodeID].nodeType = selected;
					}
				}, node.nodeType);
			}
			if (ImGui::MenuItem("Delete Node"))
			{
				DeleteNode(nodeID);
				treeModified = true;
			}

			ImGui::EndPopup();
		}

		if (node.IsSelected)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				for (SelectedNode& selected : m_SelectedNodes)
					if (!selected.IsTombstone)
						DeleteNode(selected.ID);

				treeModified = true;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_F2))
			{
				m_RenamingNodeID = nodeID;
				strncpy_s(RenameBuffer, tag.Name.c_str(), sizeof(RenameBuffer) - 1);
				RenameBuffer[sizeof(RenameBuffer) - 1] = '\0';
			}
		}
	}

	ImGui::PopID();

	// Show children if tree expanded
	if (node.IsExpanded)
	{
		if (!treeModified)
		{
			if (nodeID != 0 && node.entity.template HasComponent<ExtStemComponent>())
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

			HierarchyNode::NodeID childNodes = node.FirstChildID;
			while (childNodes != InvalidID)
			{
				if (!RenderNode(childNodes))
				{
					treeModified = true;
					break;
				}
				childNodes = m_Hierarchy[childNodes].NextSiblingID;
			}

			if (nodeID != 0 && node.entity.template HasComponent<ExtStemComponent>())
				ImGui::PopStyleColor();
		}
		ImGui::TreePop();
	}

	return !treeModified;
}

bool HierarchyPanel::AddNode(Entity entity, HierarchyNode::NodeID parentID, HierarchyNode::NodeID* idOut)
{
	if (parentID >= m_Hierarchy.size())
		return false;

	HierarchyNode::NodeID newID = InvalidID;

	// Add Entity
	if (m_TombstoneNodeCount == 0)
	{
		newID = (HierarchyNode::NodeID)m_Hierarchy.size();
		m_Hierarchy.emplace_back(entity, parentID);
	}
	else
	{
		for (HierarchyNode::NodeID i = 0; i < m_Hierarchy.size(); i++)
		{
			if (m_Hierarchy[i].IsTombstone)
			{
				newID = i;
				m_Hierarchy[i] = HierarchyNode(entity, parentID);
				m_TombstoneNodeCount--;
				break;
			}
		}
	}

	// Parent Node
	HierarchyNode& parent = m_Hierarchy[parentID];
	if (parent.FirstChildID == InvalidID)
		parent.FirstChildID = newID;
	else
	{
		HierarchyNode::NodeID lastSibling = parent.FirstChildID;
		while (m_Hierarchy[lastSibling].NextSiblingID != InvalidID)
			lastSibling = m_Hierarchy[lastSibling].NextSiblingID;
		m_Hierarchy[lastSibling].NextSiblingID = newID;
	}

	if (entity.template HasComponent<ExtStemComponent>())
		m_Hierarchy[newID].IsExpanded = false; // Default Close other Stems

	if (idOut)
		*idOut = newID;

	return newID;
}

HierarchyNode::NodeID HierarchyPanel::AddStem(Entity entity)
{
	HierarchyNode::NodeID newID = InvalidID;

	// Add Entity
	if (m_TombstoneNodeCount == 0)
	{
		newID = (HierarchyNode::NodeID)m_Hierarchy.size();
		m_Hierarchy.emplace_back(entity, InvalidID);
	}
	else
	{
		for (HierarchyNode::NodeID i = 0; i < m_Hierarchy.size(); i++)
		{
			if (m_Hierarchy[i].IsTombstone)
			{
				newID = i;
				m_Hierarchy[i] = HierarchyNode(entity, InvalidID);
				m_TombstoneNodeCount--;
				break;
			}
		}
	}

	return newID;
}

void HierarchyPanel::DeleteNode(HierarchyNode::NodeID nodeID)
{
	// Remove Node from Hierarchy
	HierarchyNode& node = m_Hierarchy[nodeID];
	HierarchyNode& parent = m_Hierarchy[node.ParentID];
	if (parent.FirstChildID == nodeID)
		parent.FirstChildID = node.NextSiblingID;
	else
	{
		HierarchyNode::NodeID prevSibling = parent.FirstChildID;
		while (m_Hierarchy[prevSibling].NextSiblingID != nodeID)
			prevSibling = m_Hierarchy[prevSibling].NextSiblingID;
		m_Hierarchy[prevSibling].NextSiblingID = node.NextSiblingID;
	}

	// Delete all child nodes (recursively)
	HierarchyNode::NodeID childNodes = node.FirstChildID;
	while (childNodes != InvalidID)
	{
		DeleteNode(childNodes);
		childNodes = node.FirstChildID;
	}

	// "Remove" from list - index can be reused
	m_Hierarchy[nodeID].IsTombstone = true;
	m_TombstoneNodeCount++;

	// Destroy Entity
	node.entity.Destroy();

	// Deselect Node
	RemoveSelectedNode(nodeID);
	if (m_LastSelectedNodeID == nodeID)
		m_LastSelectedNodeID = InvalidID;
}

void HierarchyPanel::DeleteStem(HierarchyNode::NodeID nodeID)
{
	// Remove Node from Hierarchy
	HierarchyNode& node = m_Hierarchy[nodeID];

	// Delete all child nodes (recursively)
	HierarchyNode::NodeID childNodes = node.FirstChildID;
	while (childNodes != InvalidID)
	{
		DeleteNode(childNodes);
		childNodes = node.FirstChildID;
	}

	// "Remove" from list - index can be reused
	m_Hierarchy[nodeID].IsTombstone = true;
	m_TombstoneNodeCount++;

	// Destroy Entity
	node.entity.Destroy();

	// Deselect Node
	RemoveSelectedNode(nodeID);
	if (m_LastSelectedNodeID == nodeID)
		m_LastSelectedNodeID = InvalidID;
}

void HierarchyPanel::MoveNode(HierarchyNode::NodeID nodeID, HierarchyNode::NodeID newParentID)
{
	if (nodeID == newParentID || m_Hierarchy[nodeID].ParentID == newParentID)
		return;

	HierarchyNode& node = m_Hierarchy[nodeID];
	HierarchyNode& oldParent = m_Hierarchy[node.ParentID];
	HierarchyNode& newParent = m_Hierarchy[newParentID];

	// Change Node paths
	TagComponent& tag = node.entity.template GetComponent<TagComponent>();
	TagComponent& newParentTag = newParent.entity.template GetComponent<TagComponent>();
	std::string newPath = newParentTag.NodePath + newParentTag.Name + '/';
	tag.NodePath = newPath;

	AssignChildName(newParent, nodeID, tag.Name);
	std::string oldPath = tag.NodePath + tag.Name;
	newPath += tag.Name;

	// Set Parent
	tag.Parent = newParent.entity;

	// Orphan the node from old parent
	if (oldParent.FirstChildID == nodeID)
		oldParent.FirstChildID = node.NextSiblingID;
	else
	{
		HierarchyNode::NodeID prevSibling = oldParent.FirstChildID;
		while (m_Hierarchy[prevSibling].NextSiblingID != nodeID)
			prevSibling = m_Hierarchy[prevSibling].NextSiblingID;
		m_Hierarchy[prevSibling].NextSiblingID = node.NextSiblingID;
	}

	// Set new parent
	node.ParentID = newParentID;
	node.NextSiblingID = InvalidID;

	// Adopt the node to new parent
	if (newParent.FirstChildID == InvalidID)
		newParent.FirstChildID = nodeID;
	else
	{
		HierarchyNode::NodeID lastSibling = newParent.FirstChildID;
		while (m_Hierarchy[lastSibling].NextSiblingID != InvalidID)
			lastSibling = m_Hierarchy[lastSibling].NextSiblingID;
		m_Hierarchy[lastSibling].NextSiblingID = nodeID;
	}

	// Update paths of children
	UpdateChildPaths(node, oldPath, newPath);

	// Update Global transforms (if applicable) of node
	if (node.entity.template HasComponent<TransformComponent>())
	{
		TransformComponent& transform = node.entity.template GetComponent<TransformComponent>();
		if (newParent.entity.template HasComponent<TransformComponent>())
		{
			TransformComponent& parentTransform = newParent.entity.template GetComponent<TransformComponent>();
			transform.Position = transform.GlobalPosition - parentTransform.GlobalPosition;
			transform.Rotation = transform.GlobalRotation - parentTransform.GlobalRotation;
			transform.Scale = transform.GlobalScale / parentTransform.GlobalScale;
		}
		else
		{
			transform.Position = transform.GlobalPosition;
			transform.Rotation = transform.GlobalRotation;
			transform.Scale = transform.GlobalScale;
		}
	}
}

void HierarchyPanel::RenameNode(HierarchyNode& node, const std::string& newName) {
	TagComponent& tag = node.entity.template GetComponent<TagComponent>();

	// Store old full path before renaming
	std::string oldPath = tag.NodePath + tag.Name;
	std::string newPath = tag.NodePath + newName;

	// Update node name
	tag.Name = newName;

	// Recursively update paths of children
	UpdateChildPaths(node, oldPath, newPath);
}

HierarchyNode::NodeID HierarchyPanel::GetFirstSelectedNode()
{
	if (m_SelectedNodesCount < 1)
		return InvalidID;

	for (uint32_t i = 0; i < m_SelectedNodes.size(); i++)
		if (!m_SelectedNodes[i].IsTombstone && m_SelectedNodes[i].ID < m_Hierarchy.size())
			return m_SelectedNodes[i].ID;

	return InvalidID;
}

void HierarchyPanel::UpdateChildPaths(HierarchyNode& node, const std::string& oldPath, const std::string& newPath)
{
	HierarchyNode::NodeID childID = node.FirstChildID;

	while (childID != InvalidID)
	{
		HierarchyNode& childNode = m_Hierarchy[childID];
		TagComponent& childTag = childNode.entity.template GetComponent<TagComponent>();

		// Replace old path with new path in child's NodePath
		YEASTEM_ASSERT(childTag.NodePath.find(oldPath) != std::string::npos, "Child node does not contain Node Path!");
		childTag.NodePath = newPath + childTag.NodePath.substr(oldPath.length());

		// Recursively update deeper levels
		UpdateChildPaths(childNode, oldPath, newPath);

		// Move to the next sibling
		childID = childNode.NextSiblingID;
	}
}

HierarchyNode::NodeID HierarchyPanel::GetChildByName(HierarchyNode& node, const std::string& nodeName)
{
	HierarchyNode::NodeID childID = node.FirstChildID;
	while (childID != InvalidID)
	{
		HierarchyNode& childNode = m_Hierarchy[childID];
		TagComponent& childTag = childNode.entity.template GetComponent<TagComponent>();
		if (nodeName == childTag.Name)
			return childID;
		
		childID = childNode.NextSiblingID;
	}

	return InvalidID;
}

int HierarchyPanel::GetNumericalSuffix(std::string& name, bool stripString = false)
{
	int result = 0;

	size_t i;
	size_t length = name.size();
	for (i = 0; i < length; i++)
	{
		size_t j = length - i - 1;
		if (!std::isdigit(name[j]))
		{
			if (i == 0) return -1; // No number found
			break;
		}

		result += (name[j] - '0') * (int)pow(10u, i);
	}

	if (stripString)
		name.erase(length - i, i);

	return (int)result;
}

void HierarchyPanel::AssignChildName(const HierarchyNode& parent, HierarchyNode::NodeID childID, std::string& childName)
{
	// Multiple lines needed, as GetNumericalSuffix() will modify {strip} the string
	uint32_t numberExtension = GetNumericalSuffix(childName, true);
	if (numberExtension != -1)
		AssignChildName(parent, childID, childName, numberExtension, true);
	else
		AssignChildName(parent, childID, childName, 0, false);	
}

void HierarchyPanel::AssignChildName(
	const HierarchyNode& parent, HierarchyNode::NodeID searchChildID, 
	std::string& searchChildName, uint32_t numberExtension, bool forceNumberExtension
) {
	// Set the name to the current node name (modified by the current numberExtension test)
	std::string name = searchChildName;
	if (numberExtension > 1 || forceNumberExtension)
		name += std::to_string(numberExtension);
	
	// Check if the name already exists in the parent node
	HierarchyNode::NodeID childID = parent.FirstChildID;
	while (childID != InvalidID)
	{
		HierarchyNode& childNode = m_Hierarchy[childID];
		TagComponent& childTag = childNode.entity.template GetComponent<TagComponent>();
		if (name == childTag.Name && childID != searchChildID)
			return AssignChildName(parent, searchChildID, searchChildName, numberExtension + 1, forceNumberExtension);
		
		childID = childNode.NextSiblingID;
	}

	// If it's not found, append the extension to the node
	if (numberExtension > 1 || forceNumberExtension)
		searchChildName += std::to_string(numberExtension);
}

void HierarchyPanel::Clear()
{
	ClearSelectedNodes();
	for (HierarchyNode::NodeID i = 0; i < m_Hierarchy.size(); i++)
	{
		if (!m_Hierarchy[i].IsTombstone)
			m_Hierarchy[i].entity.Destroy();
	}
	m_Hierarchy.clear();
	m_TombstoneNodeCount = 0;
}

YEASTEM_END
