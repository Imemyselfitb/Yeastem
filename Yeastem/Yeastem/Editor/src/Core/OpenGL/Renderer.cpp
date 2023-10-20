#include "yst_pch.h"

#include "Renderer.h"

YEASTEM_BEGIN

#define DestoryUniquePtr(ptr) ptr.~unique_ptr();

void Renderer::AssignIndexBuffer(const unsigned int* data, unsigned int count)
{
	DestoryUniquePtr(this->m_IndexBuffer);
	this->m_IndexBuffer = std::make_unique<IndexBuffer>(data, count);
}

void Renderer::AssignVertexBuffer(const void* data, unsigned int size)
{
	DestoryUniquePtr(this->m_VertexBuffer);
	this->m_VertexBuffer = std::make_unique<VertexBuffer>(data, size);
}

void Renderer::AssignShader(const Shader& other)
{
	this->m_Shader = other;
}

void Renderer::AssignShaderFromFiles(const std::string& vert, const std::string& frag)
{
	this->m_Shader.AssignShaderFromFiles(vert, frag);
}


YEASTEM_END
