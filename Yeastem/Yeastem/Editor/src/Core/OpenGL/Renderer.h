#pragma once

#include "Shader.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Texture.h"

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
	void AddTexture(const std::string&);

	void ReserveTextures(unsigned int count);

	void AddVertexBufferLayout(const VertexBufferLayout&);

	void BindShader() { this->m_Shader.Bind(); }
	void BindVertexBuffer() { this->m_VertexBuffer->Bind(); }
	void BindVertexArray() { this->m_VertexArray->Bind(); }
	void BindIndexBuffer() { this->m_IndexBuffer->Bind(); }
	void BindTextureToSlot(unsigned int idx, unsigned int slot = 0);
	void BindTexture(unsigned int idx);

	void UnbindTextures();

	const IndexBuffer& getIndexBuffer() const { return *this->m_IndexBuffer; }
	const VertexBuffer& getVertexBuffer() const { return *this->m_VertexBuffer; }
	const Shader& getShader() const { return this->m_Shader; }

	void SetUniform1i(const std::string& name, int v0) 
	{ this->m_Shader.SetUniform1i(name, v0); }
	void SetUniform1f(const std::string& name, float v0) 
	{ this->m_Shader.SetUniform1f(name, v0); }
	void SetUniform4i(const std::string& name, int v0, int v1, int v2, int v3) 
	{ this->m_Shader.SetUniform4i(name, v0, v1, v2, v3); }
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) 
	{ this->m_Shader.SetUniform4f(name, v0, v1, v2, v3); }

private:
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<VertexArray> m_VertexArray;
	Shader m_Shader;

	std::vector<Texture> m_Texture;
	unsigned int m_TextureSlotsUsed[30] = { 0 };
};

YEASTEM_END
