#pragma once

#include "Panels/NodeSelect.h"

YEASTEM_BEGIN

struct HierarchyNode
{
	using NodeID = uint32_t;

	NodeID ParentID = -1;
	NodeID FirstChildID = -1;
	NodeID NextSiblingID = -1;

	NodeType nodeType = NodeType::None;

	bool IsTombstone = false; // Has Been Deleted (so can be reused)
	bool IsExpanded = true;
	bool IsSelected = false;

	Entity entity;

	HierarchyNode(Entity entityIn, NodeID parentID = -1)
		:entity(entityIn), ParentID(parentID), FirstChildID(-1), NextSiblingID(-1) {}
};

struct SelectedNode
{
	bool IsTombstone = false; // Has Been Deleted (so can be reused)
	HierarchyNode::NodeID ID = -1;

	SelectedNode() {}
	SelectedNode(HierarchyNode::NodeID id, bool isTombstone = false) : ID(id), IsTombstone(isTombstone) {}
	SelectedNode(const SelectedNode& other) : ID(other.ID), IsTombstone(other.IsTombstone) {}
};

class HierarchyPanel
{
public:
	HierarchyPanel() = default;

	void Init(Scene& scene);
	void Render(bool& isWindowOpened);
	bool AddNode(Entity entity, HierarchyNode::NodeID parentID, HierarchyNode::NodeID* idOut = nullptr);
	void DeleteNode(HierarchyNode::NodeID parentID);
	void MoveNode(HierarchyNode::NodeID nodeID, HierarchyNode::NodeID newParentID);
	void RenameNode(HierarchyNode& node, const std::string& newName);

	HierarchyNode::NodeID GetFirstSelectedNode();
	HierarchyNode& GetNode(HierarchyNode::NodeID nodeID) { return m_Hierarchy[nodeID]; }

public:
	std::vector<HierarchyNode> m_Hierarchy;
	NodeSelectPanel nodeSelectPanel;

private:
	bool RenderNode(HierarchyNode::NodeID node);
	void OnNodeClicked(HierarchyNode::NodeID node, const char* nodeName);
	void UpdateChildPaths(HierarchyNode& node, const std::string& oldPath, const std::string& newPath);
	HierarchyNode::NodeID GetChildByName(HierarchyNode& node, const std::string& nodeName);
	void AssignChildName(const HierarchyNode& parent, HierarchyNode::NodeID childID, std::string& childName);
	void AssignChildName(
		const HierarchyNode& parent, HierarchyNode::NodeID childID, 
		std::string& childName, uint32_t numberExtension, bool forceNumberExtension
	);
	int GetNumericalSuffix(std::string& name, bool stripString);

	void AddSelectedNode(HierarchyNode::NodeID nodeID);
	void AddSelectedNodeChildren(HierarchyNode::NodeID nodeID);
	void AddSelectedNodeRange(HierarchyNode::NodeID startID, HierarchyNode::NodeID endID);
	void RemoveSelectedNode(HierarchyNode::NodeID nodeID);
	bool IsNodeSelected(HierarchyNode::NodeID nodeID);
	void ClearSelectedNodes();

private:
	const uint32_t m_MinSelectedNodeCount = 10;
	uint32_t m_TombstoneSelectedNodeCount = 0;
	HierarchyNode::NodeID m_LastSelectedNodeID = -1;
	std::vector<SelectedNode> m_SelectedNodes;
	uint32_t m_SelectedNodesCount = 0; // Living

	uint32_t m_TombstoneNodeCount = 0;
	HierarchyNode::NodeID m_RenamingNodeID = -1;
};

YEASTEM_END
