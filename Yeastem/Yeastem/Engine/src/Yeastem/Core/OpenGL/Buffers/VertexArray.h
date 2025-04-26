#pragma once

#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

#include "IndexBuffer.h"

YEASTEM_BEGIN

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;

	const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	void SetIndexBuffer(std::shared_ptr<IndexBuffer> IBO);
	
public:
	uint32_t m_glID = 0;

private:
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

YEASTEM_END
