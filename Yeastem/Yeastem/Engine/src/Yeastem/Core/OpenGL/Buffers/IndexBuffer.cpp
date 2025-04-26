#include "yst_pch.h"

#include "IndexBuffer.h"

YEASTEM_BEGIN

IndexBuffer::IndexBuffer(const IndexBuffer::Type* data, uint32_t count)
{
	size = count;
	glGenBuffers(1, &m_glID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(IndexBuffer::Type), data, GL_DYNAMIC_DRAW);
	std::cout << "Yestem: Graphics: Index Buffer Created!" << std::endl;
}

IndexBuffer::IndexBuffer()
{
	glGenBuffers(1, &m_glID);
	std::cout << "Yestem: Graphics: Index Buffer Created!" << std::endl;
}

IndexBuffer::~IndexBuffer()
{
	std::cout << "Yestem: Graphics: Index Buffer Deleted!" << std::endl;
	glDeleteBuffers(1, &m_glID);
}

void IndexBuffer::Bind() const 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glID);
}

void IndexBuffer::Unbind() const 
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void IndexBuffer::UpdateBuffer(const void* data) const
{
	Bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size * sizeof(IndexBuffer::Type), data);
	Unbind();
}

void IndexBuffer::UpdateBuffer(const void* data, uint32_t updateSize) const
{
	Bind();
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, updateSize * sizeof(IndexBuffer::Type), data);
	Unbind();
}

YEASTEM_END
