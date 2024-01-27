#include "yst_pch.h"

#include "IndexBuffer.h"

YEASTEM_BEGIN

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
{
	this->size = count;
	glGenBuffers(1, &this->m_glID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_glID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
	std::cout << "Yestem: Graphics: Index Buffer Created!" << std::endl;
}

IndexBuffer::~IndexBuffer()
{
	std::cout << "Yestem: Graphics: Index Buffer Deleted!" << std::endl;
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

void IndexBuffer::UpdateBuffer(const void* data) const
{
	this->Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
	this->Unbind();
}

void IndexBuffer::UpdateBuffer(const void* data, unsigned int count)
{
	this->size = count;
	this->Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->size * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
	this->Unbind();
}

YEASTEM_END
