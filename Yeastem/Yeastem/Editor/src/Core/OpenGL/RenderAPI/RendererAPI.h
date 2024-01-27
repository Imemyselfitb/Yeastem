#pragma once

#include "Core/OpenGL/Buffers/IndexBuffer.h"
#include "Core/OpenGL/Buffers/VertexBuffer.h"
#include "Core/OpenGL/Buffers/VertexArray.h"

#include "Core/OpenGL/Shader.h"
#include "Core/OpenGL/Texture.h"

#include "Core/Math/Vector2.h"

YEASTEM_BEGIN

struct Vertex
{
	Vector2 position = {}, texture = {};
};

class Renderable
{
public:
	Renderable() {}
	~Renderable() {}

public:
	void AssignVertexBuffer(const void* data, unsigned int size);
	void AssignVertices(const std::vector<Vertex>& vertexList);
	void AssignIndexBuffer(const unsigned int* data, unsigned int count);
	void AddVertexBufferLayout(const VertexBufferLayout& layout);
	void AssignShaderFromFiles(const std::string& VertexShader, const std::string& FragmentShader);

	void ReserveTextures(unsigned int count);
	void AddTexture(const std::string& path);

	void ReloadIndices();
	void ReloadVertices();

public:
	std::vector<Vertex> VertexList;
	std::shared_ptr<std::vector<unsigned int>> IndexList;
	std::unique_ptr<VertexBuffer> vertices;
	std::unique_ptr<IndexBuffer> indices;
	std::unique_ptr<VertexArray> layout;
	
	std::shared_ptr<Shader> shader;
	std::vector<Texture> textures;
};

class BasicRenderer
{
public:
	BasicRenderer() {}

	void Render(const Renderable& obj);
};

YEASTEM_END
