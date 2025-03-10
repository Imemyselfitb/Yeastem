#include "yst_pch.h"

#include "VertexBuffer.h"

YEASTEM_BEGIN

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &m_glID);
	std::cout << "Yestem: Graphics: Vertex Buffer Created!" << std::endl;
}

VertexBuffer::VertexBuffer(const void* data, uint32_t size)
{
	glGenBuffers(1, &m_glID);
	glBindBuffer(GL_ARRAY_BUFFER, m_glID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

	m_Size = size;
	std::cout << "Yestem: Graphics: Vertex Buffer Created!" << std::endl;
}

VertexBuffer::~VertexBuffer()
{
	std::cout << "Yestem: Graphics: Vertex Buffer Deleted!" << std::endl;
	glDeleteBuffers(1, &m_glID);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_glID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::UpdateBuffer(const void* data) const
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_Size, data);
	Unbind();
}

void VertexBuffer::UpdateBuffer(const void* data, uint32_t size) const
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	Unbind();
}

YEASTEM_END
