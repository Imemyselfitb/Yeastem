#include "yst_pch.h"

#include "Shape.h"

// TODO: Add to AssignVertices();

YEASTEM_BEGIN

void Shape::Init()
{
	this->m_RenderableShape = std::make_shared<Renderable>();
}

void Shape::AssignVertices(const std::vector<Vertex>& vertices)
{
	this->m_RenderableShape->AssignVertices(vertices);
	for (int i = 0; i < vertices.size(); i++)
	{
		this->m_Points.emplace_back(vertices[i].position);
		this->m_Points[i].m_VertexListIndex = i;
	}
	
	Vector2 centre = this->GetCentre();
	for (PointMass& pt : this->m_Points)
		pt.SetOffset(centre);
}

void Shape::AssignIndexBuffer(const unsigned int* data, unsigned int count)
{
	this->m_RenderableShape->AssignIndexBuffer(data, count);
}
void Shape::AddVertexBufferLayout(const VertexBufferLayout& layout)
{
	this->m_RenderableShape->AddVertexBufferLayout(layout);
}
void Shape::AssignShaderFromFiles(const std::string& VertexShader, const std::string& FragmentShader)
{
	this->m_RenderableShape->AssignShaderFromFiles(VertexShader, FragmentShader);
	this->shader = this->m_RenderableShape->shader;
}
void Shape::ReserveTextures(unsigned int count)
{
	this->m_RenderableShape->ReserveTextures(count);
}
void Shape::AddTexture(const std::string& path)
{
	this->m_RenderableShape->AddTexture(path);
}

void Shape::UpdateVertices(int windowWidth, int windowHeight)
{
	for (PointMass& pt : this->m_Points)
	{
		if (!pt.m_RenderableShape)
			pt.m_RenderableShape = this->m_RenderableShape;

		pt.SubmitToVertexList(windowWidth, windowHeight);
	}

	this->m_RenderableShape->ReloadVertices();
}

Vector2 Shape::GetCentre()
{
	Vector2 Center;
	for (const PointMass& pt : this->m_Points)
	{
		Center += pt.Position;
	}
	return Center / (float)this->m_Points.size();
}

YEASTEM_END
