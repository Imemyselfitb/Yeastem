#include "yst_pch.h"

#include "VertexArray.h"

YEASTEM_BEGIN

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &this->m_glID);
	glBindVertexArray(this->m_glID);
}

void VertexArray::AddBuffer(
	const VertexBuffer& vertex, 
	const VertexBufferLayout& layout
) {
	vertex.Bind();
	const std::vector<VertexBufferLayoutElement>& Elements = layout.getElements();
	
	unsigned int offset = 0;
	for (unsigned int i = 0; i < Elements.size(); i++)
	{
		const VertexBufferLayoutElement& e = Elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i, e.count, e.type, e.normalized,
			layout.getStride(), (const void*) offset
		);
		offset += e.count * e.openGlTypeSize;
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(this->m_glID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &this->m_glID);
}

unsigned int GetSizeOfGLType(unsigned int type)
{
	switch (type)
	{
	case GL_FLOAT: return 4;
	case GL_UNSIGNED_INT: return 4;
	case GL_UNSIGNED_BYTE: return 1;
	};

	YEASTEM_WARNING("OpenGL Type " << type << " not supported. ");
	return 0;
}

YEASTEM_END
