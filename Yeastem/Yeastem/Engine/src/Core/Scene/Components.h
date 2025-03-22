#pragma once

#include "Core/Math/Vector2.h"
#include "Core/OpenGL/Buffers/IndexBuffer.h"

YEASTEM_BEGIN

struct TagComponent
{
	const char* NodePath = "";
	const char* Name = "";

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const char* nodePath, const char* name) :NodePath(nodePath), Name(name) {}
};

struct TransformComponent
{
	Vector2 Position{ 0.0f, 0.0f };
	float Rotation = 0.0f;
	float Scale = 1.0f;
	
	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(Vector2 position, float rotation = 0.0f, float scale = 1.0f)
		:Position(position), Rotation(rotation), Scale(scale) {}

	Vector2 TransformPoint(Vector2 pos) const
	{
		return Position + (pos.RotateNew(Rotation) * Scale);
	}
};

struct RenderQuadComponent
{
	Vector2 Size{};
	uint32_t CurrentTexture = 0;
	std::vector<ObjectID> Textures{};

	RenderQuadComponent() = default;
	RenderQuadComponent(const RenderQuadComponent&) = default;
};

struct ScriptComponent
{
	const char* FilePath = "";

	ScriptComponent() = default;
	ScriptComponent(const char* filePath) :FilePath(filePath) {}
	ScriptComponent(const ScriptComponent&) = default;
};

YEASTEM_END
