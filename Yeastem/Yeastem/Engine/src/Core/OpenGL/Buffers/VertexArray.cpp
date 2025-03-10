#include "yst_pch.h"

#include "VertexArray.h"

YEASTEM_BEGIN

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_glID);
	glBindVertexArray(m_glID);
	std::cout << "Yestem: Graphics: Vertex Array Created!" << std::endl;
}

void VertexArray::AddBuffer(
	const VertexBuffer& QuadVertex, 
	const VertexBufferLayout& layout
) {
	QuadVertex.Bind();
	const std::vector<VertexBufferLayoutElement>& Elements = layout.getElements();
	
	uint32_t offset = 0;
	for (uint32_t i = 0; i < Elements.size(); i++)
	{
		const VertexBufferLayoutElement& e = Elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i, e.count, e.type, e.normalized,
			layout.getStride(), reinterpret_cast<const void*>(static_cast<uintptr_t>(offset))
		);
		offset += e.count * e.openGlTypeSize;
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_glID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> IBO)
{
	Bind();
	IBO->Bind();
	m_IndexBuffer = IBO;
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_glID);
}

uint32_t GetSizeOfGLType(uint32_t type)
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
