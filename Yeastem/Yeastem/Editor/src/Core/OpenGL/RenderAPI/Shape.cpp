#include "yst_pch.h"

#include "Shape.h"

// TODO: Add to AssignVertices();

YEASTEM_BEGIN

void Shape::Init(Vector2 centre)
{
	this->m_RenderableShape = std::make_shared<Renderable>();
	this->m_InitPosition = centre;
}

void Shape::Push()
{
	for (PointMass& pt : this->m_Points)
		pt.Push();
}
void Shape::Translate(Vector2 v)
{
	for (PointMass& pt : this->m_Points)
		pt.Position += v;
}
void Shape::Rotate(float Angle, Vector2 Pivot)
{
	for (PointMass& pt : this->m_Points)
		pt.Position.Rotate(Angle, Pivot);
}
void Shape::ScaleBy(float sf, Vector2 centre)
{
	for (PointMass& pt : this->m_Points)
		pt.Position.Scale(sf, centre);
}
void Shape::Pop()
{
	for (PointMass& pt : this->m_Points)
		pt.Pop();
}

void Shape::AssignVertices(const std::vector<Vertex>& vertices)
{
	this->m_RenderableShape->AssignVertices(vertices);
	for (int i = 0; i < vertices.size(); i++)
	{
		this->m_Points.emplace_back(vertices[i].position);
		this->m_Points[i].m_VertexListIndex = i;
	}
	
	this->AssignOffsets();
}
void Shape::AssignOffsets()
{
	Vector2 centre = this->GetCentre();
	for (PointMass& pt : this->m_Points)
		pt.SetOffset(centre);
}

void Shape::AssignIndices(const std::vector<unsigned int>& vertices)
{
	this->m_RenderableShape->AssignIndexBuffer(&vertices[0], vertices.size());
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
	float xmin, ymin, xmax, ymax;

	float x1 = this->m_Points[0].Position.x;
	float y1 = this->m_Points[0].Position.y;

	xmin = float(x1);
	xmax = float(x1);
	ymin = float(y1);
	ymax = float(y1);

	for (PointMass& pt : this->m_Points)
	{
		if (!pt.m_RenderableShape)
			pt.m_RenderableShape = this->m_RenderableShape;

		xmin = SDL_min(xmin, pt.Position.x);
		ymin = SDL_min(ymin, pt.Position.y);
		xmax = SDL_max(xmax, pt.Position.x);
		ymax = SDL_max(ymax, pt.Position.y);

		pt.SubmitToVertexList(windowWidth, windowHeight);
	}
	
	this->Size.x = xmax - xmin;
	this->Size.y = ymax - ymin;

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
