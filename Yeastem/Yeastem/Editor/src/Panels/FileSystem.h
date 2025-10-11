#pragma once

#include "Yeastem.h"

YEASTEM_BEGIN

class FileSystemPanel
{
public:
	FileSystemPanel() = default;

public:
	void Render(bool& isOpen);
	/*
private:
	struct SelectedNode
	{
		bool IsTombstone = false; // Has Been Deleted (so can be reused)
		HierarchyNode::NodeID ID = -1;

		SelectedNode() {}
		SelectedNode(HierarchyNode::NodeID id, bool isTombstone = false) : ID(id), IsTombstone(isTombstone) {}
		SelectedNode(const SelectedNode& other) : ID(other.ID), IsTombstone(other.IsTombstone) {}
	};

private:
	void AddSelectedNode(HierarchyNode::NodeID id, bool isTombstone = false);
	*/
};

YEASTEM_END
