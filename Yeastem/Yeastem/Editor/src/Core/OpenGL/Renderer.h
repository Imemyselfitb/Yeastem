#pragma once

#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

YEASTEM_BEGIN

class Renderer
{
public:
	Renderer() {}
	~Renderer() {}

public:
	void AssignIndexBuffer(const unsigned int* data, unsigned int count);
	void AssignVertexBuffer(const void* data, unsigned int size);
	void AssignShader(const Shader&);
	void AssignShaderFromFiles(const std::string&, const std::string&);

	void BindShader() { this->m_Shader.Bind(); }
	void BindVertexBuffer() { this->m_VertexBuffer->Bind(); }
	void BindIndexBuffer() { this->m_IndexBuffer->Bind(); }

	const IndexBuffer& getIndexBuffer() const { return *this->m_IndexBuffer; }
	const VertexBuffer& getVertexBuffer() const { return *this->m_VertexBuffer; }
	const Shader& getShader() const { return this->m_Shader; }

private:
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	Shader m_Shader;
};

YEASTEM_END
