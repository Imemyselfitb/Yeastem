#pragma once

#include "Core/OpenGL/Buffers/IndexBuffer.h"
#include "Core/OpenGL/Buffers/VertexBuffer.h"
#include "Core/OpenGL/Buffers/VertexArray.h"

#include "Core/OpenGL/Shader.h"
#include "Core/OpenGL/Texture.h"

YEASTEM_BEGIN

class Renderable
{
public:
	Renderable() {}
	~Renderable() {}

public:
	void AssignVertexBuffer(const void* data, unsigned int size);
	void AssignIndexBuffer(const unsigned int* data, unsigned int count);
	void AddVertexBufferLayout(const VertexBufferLayout& layout);
	void AssignShaderFromFiles(const std::string& VertexShader, const std::string& FragmentShader);

	void ReserveTextures(unsigned int count);
	void AddTexture(const std::string& path);

public:
	std::unique_ptr<VertexBuffer> vertices;
	std::unique_ptr<IndexBuffer> indices;
	std::unique_ptr<VertexArray> layout;
	
	Shader shader;
	std::vector<Texture> textures;
};

class BasicRenderer
{
public:
	BasicRenderer() {}

	void Render(const Renderable& obj);
};

YEASTEM_END
