#include "yst_pch.h"

#include "IndexBuffer.h"

YEASTEM_BEGIN

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
{
	this->m_Count = count;
	glGenBuffers(1, &this->m_glID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_glID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	std::cout << "Index Buffer Created!" << std::endl;
}

IndexBuffer::~IndexBuffer()
{
	std::cout << "Index Buffer Deleted!" << std::endl;
	glDeleteBuffers(1, &this->m_glID);
}

void IndexBuffer::Bind() const 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_glID);
}

void IndexBuffer::Unbind() const 
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

YEASTEM_END
