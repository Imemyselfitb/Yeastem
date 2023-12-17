#include "yst_pch.h"

#include "VertexBuffer.h"

YEASTEM_BEGIN

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	glGenBuffers(1, &this->m_glID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_glID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

	this->m_Size = size;
	std::cout << "Yestem: Graphics: Vertex Buffer Created!" << std::endl;
}

VertexBuffer::~VertexBuffer()
{
	std::cout << "Yestem: Graphics: Vertex Buffer Deleted!" << std::endl;
	glDeleteBuffers(1, &this->m_glID);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->m_glID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::UpdateBuffer(const void* data) const
{
	this->Bind();
	glBufferData(GL_ARRAY_BUFFER, this->m_Size, data, GL_DYNAMIC_DRAW);
	this->Unbind();
}

YEASTEM_END
