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
	void AssignIndexBuffer(const unsigned int* data, unsigned int count);
	void AddVertexBufferLayout(const VertexBufferLayout& layout);
	void AssignShaderFromFiles(const std::string& VertexShader, const std::string& FragmentShader);

	void ReserveTextures(unsigned int count);
	void AddTexture(const std::string& path);

	operator Renderable& () { return *this->m_RenderableShape.get(); }
	operator const Renderable& () const { return *this->m_RenderableShape.get(); }

public:
	std::vector<PointMass>& GetPoints() { return m_Points; }
	const std::vector<PointMass>& GetPoints() const { return m_Points; }

	void Init();
	void UpdateVertices(int windowWidth, int windowHeight);

public:
	Vector2 GetCentre();

public:
	std::shared_ptr<Shader> shader;

private:
	std::vector<PointMass> m_Points;
	std::shared_ptr<Renderable> m_RenderableShape;
};

YEASTEM_END
