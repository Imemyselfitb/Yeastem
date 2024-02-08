#pragma once

#include "Core/OpenGL/RenderAPI/RendererAPI.h"
#include "Core/Math/Point.h"

YEASTEM_BEGIN

class Shape
{
public:
	Shape() {}

public:
	void AssignVertices(const std::vector<Vertex>& vertices);
	void AssignIndices(const std::vector<unsigned int>& vertices);
	void AssignIndexBuffer(const unsigned int* data, unsigned int count);
	void AddVertexBufferLayout(const VertexBufferLayout& layout);
	void AssignShaderFromFiles(const std::string& VertexShader, const std::string& FragmentShader);

	void ReserveTextures(unsigned int count);
	void AddTexture(const std::string& path);

	operator Renderable& () { return *this->m_RenderableShape.get(); }
	operator const Renderable& () const { return *this->m_RenderableShape.get(); }

public:
	std::vector<PointMass>& GetPoints() { return m_Points; }

	void Init(Vector2 centre);
	void AssignOffsets();
	void Push();
	void Translate(Vector2 v);
	void Rotate(float Angle, Vector2 Pivot = { 0, 0 });
	void ScaleBy(float ScaleFactor, Vector2 centre = { 0,0 });
	void Pop();
	void UpdateVertices(int windowWidth, int windowHeight);

	const std::vector<PointMass>& GetPoints() const { return m_Points; }
	Vector2 GetCentre();

public:
	Vector2 Size;
	Vector2 Position;
	float Direction = 0.0f;
	float Scale = 1.0f;

public:
	std::shared_ptr<Shader> shader;

private:
	Vector2 m_InitPosition;
	std::vector<PointMass> m_Points;
	std::shared_ptr<Renderable> m_RenderableShape;
};

YEASTEM_END
