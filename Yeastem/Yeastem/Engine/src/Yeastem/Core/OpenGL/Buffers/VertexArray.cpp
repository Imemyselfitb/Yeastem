#include "yst_pch.h"

#include "VertexArray.h"

YEASTEM_BEGIN

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_glID);
	glBindVertexArray(m_glID);
	std::cout << "Yestem: Graphics: Vertex Array Created!" << std::endl;
}

void VertexArray::AddBuffer(const VertexBuffer& QuadVertex, const VertexBufferLayout& layout)
{
	QuadVertex.Bind();
	const std::vector<VertexBufferLayoutElement>& Elements = layout.getElements();
	
	uint32_t offset = 0;
	for (uint32_t i = 0; i < Elements.size(); i++)
	{
		const VertexBufferLayoutElement& e = Elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i, e.count, e.type, e.normalized,
			layout.getStride(), (const void*)(uintptr_t)offset
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

YEASTEM_END
