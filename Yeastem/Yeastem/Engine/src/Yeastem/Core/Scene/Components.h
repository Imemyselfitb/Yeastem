#pragma once

#include "Yeastem/Core/Math/Vector2.h"
#include "Yeastem/Core/OpenGL/Buffers/IndexBuffer.h"

#include <entt.hpp>

YEASTEM_BEGIN

namespace ComponentTypes
{
	enum Components : uint32_t
	{
		Tag = 0, // All entities will have it (hence default)
		Transform = 1 << 0,
		RenderQuad = 1 << 1,
		Script = 1 << 2
	};
}

struct TagComponent
{
	std::string NodePath = ""; // Path TO node (does not include node name itself)
	std::string Name = "";
	entt::entity Parent = entt::null;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const char* nodePath, const char* name, entt::entity parent = entt::null)
		:NodePath(nodePath), Name(name), Parent(parent) {}
};

struct TransformComponent
{
	Vector2f Position{ 0.0f, 0.0f };
	Vector2f Scale{ 1.0f, 1.0f };
	float Rotation = 0.0f;
	float ZLevel = 0.0f;

	///TODO
	Vector2f GlobalPosition{ 0.0f, 0.0f };
	Vector2f GlobalScale{ 1.0f, 1.0f };
	float GlobalRotation = 0.0f;
	
	uint32_t FrameUpdate = 0;
	
	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(Vector2f position, float rotation = 0.0f, Vector2f scale = { 1.0f, 1.0f })
		:Position(position), Rotation(rotation), Scale(scale) {}

	Vector2f TransformPointLocal(const Vector2f& pos) const
	{
		return Position + (pos.RotateNew(Rotation) * Scale);
	}
	Vector2f TransformPoint(const Vector2f& pos) const
	{
		return GlobalPosition + (pos.RotateNew(GlobalRotation) * GlobalScale);
	}
};

struct RenderQuadComponent
{
	Vector2f Size{};
	uint32_t CurrentTexture = 0;
	std::vector<ObjectID> Textures{};

	RenderQuadComponent() = default;
	RenderQuadComponent(const RenderQuadComponent&) = default;
};

struct ScriptComponent
{
	std::string FilePath = "";

	ScriptComponent() = default;
	ScriptComponent(const char* filePath) :FilePath(filePath) {}
	ScriptComponent(const ScriptComponent&) = default;
};

YEASTEM_END
