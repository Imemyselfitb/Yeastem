#pragma once

namespace HierarchyNode
{
	using NodeID = uint32_t;
}

struct StemComponent
{
	std::filesystem::path Source;
	HierarchyNode::NodeID nodeID = -1;
	ObjectID SceneID = 0;

	StemComponent() = default;
	StemComponent(const std::filesystem::path& filePath) :Source(filePath) {}
	StemComponent(const StemComponent&) = default;
};

struct ExtStemComponent
{
	std::filesystem::path Source;

	ExtStemComponent() = default;
	ExtStemComponent(const std::filesystem::path& filePath) :Source(filePath) {}
	ExtStemComponent(const ExtStemComponent&) = default;
};
