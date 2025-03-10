#include "yst_pch.h"

#include "Shape.h"

#include "FileIO/FileIO.h"
#include "RendererAPI.h"

YEASTEM_BEGIN

template class Shape<QuadVertex>;

template<typename VertexType>
void Shape<VertexType>::AssignVertices(const std::vector<VertexType>& vertices)
{
	m_RenderableShape.VertexList.reserve(vertices.size());
	m_RenderableShape.VertexList = vertices;

	Vector2 min{ 0,0 };
	Vector2 max{ 0,0 };
	for (const VertexType& vert : vertices)
	{
		if (vert.Position.x < min.x)
			min.x = vert.Position.x;
		else if (vert.Position.x > max.x)
			max.x = vert.Position.x;

		if (vert.Position.y < min.y)
			min.y = vert.Position.y;
		else if (vert.Position.y > max.y)
			max.y = vert.Position.y;
	}
	Size = max - min;
}

template<typename VertexType>
void Shape<VertexType>::AssignIndices(const std::vector<IndexBuffer::Type>& indexListIn)
{
	m_RenderableShape.IndexList.reserve(indexListIn.size());
	m_RenderableShape.IndexList = indexListIn;
}
template<typename VertexType>
void Shape<VertexType>::ReserveTextures(uint32_t count)
{
	m_RenderableShape.Textures.reserve(count);
}
template<typename VertexType>
void Shape<VertexType>::AddTexture(const std::string& path, ResourceManager& resourceManager)
{
	if (FileIO::checkIfExists(path))
		m_RenderableShape.Textures.push_back(resourceManager.Textures.Load(path));
}

template<typename VertexType>
Vector2 Shape<VertexType>::GetVertexPosition(uint32_t vertIdx, float windowWidth, float windowHeight) const
{
	Vector2 pos = m_RenderableShape.VertexList[vertIdx].Position;
	pos.Rotate(Direction);
	pos.Scale(Scale);
	pos += Position;

	pos /= Vector2(windowWidth * 0.5f, windowHeight * -0.5f);
	pos += Vector2(-1.0f, 1.0f);
	return pos;
}

template<typename VertexType>
Vector2 Shape<VertexType>::GetCentre() const
{
	Vector2 Center;
	uint32_t verticesCount = GetVerticesCount();
	for (uint32_t i = 0; i < verticesCount; i++)
		Center += m_RenderableShape.VertexList[i].Position;
	
	return Center / static_cast<float>(verticesCount);
}

YEASTEM_END
