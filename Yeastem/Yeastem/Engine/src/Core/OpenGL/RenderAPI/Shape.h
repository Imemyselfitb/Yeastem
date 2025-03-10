#pragma once

#include "Core/OpenGL/RenderAPI/RendererAPI.h"
#include "Core/Math/Vector2.h"

YEASTEM_BEGIN

template <typename VertexType>
class Shape
{
public:
	Shape() :m_RenderableShape() {}

public:
	void AssignVertices(const std::vector<VertexType>& vertices);
	void AssignIndices(const std::vector<IndexBuffer::Type>& vertices);
	void ReserveTextures(uint32_t count);
	void AddTexture(const std::string& path, ResourceManager& resourceManager);

	Renderable<VertexType>& GetRenderable() { return m_RenderableShape; }
	const Renderable<VertexType>& GetRenderable() const { return m_RenderableShape; }

	Vector2 GetVertexPosition(uint32_t vertIdx, float windowWidth, float windowHeight) const;

	Vector2 GetCentre() const;
	uint32_t GetVerticesCount() const { return static_cast<uint32_t>(m_RenderableShape.VertexList.size()); }

public:
	Vector2 Size{ 0.0f, 0.0f };
	Vector2 Position{ 0.0f, 0.0f };
	float Direction = 0.0f;
	float Scale = 1.0f;

	Vector2 DefaultPosition{ 0.0f, 0.0f };
	float DefaultDirection = 0.0f;
	float DefaultScale = 1.0f;

private:
	Renderable<VertexType> m_RenderableShape;
};

YEASTEM_END
