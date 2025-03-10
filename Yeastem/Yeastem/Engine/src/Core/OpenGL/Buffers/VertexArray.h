#pragma once

#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

#include "Core/OpenGL/Buffers/IndexBuffer.h"

YEASTEM_BEGIN

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(
		const VertexBuffer&, const VertexBufferLayout&
	);

	void Bind() const;
	void Unbind() const;

	void SetIndexBuffer(std::shared_ptr<IndexBuffer> IBO);
	const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	
public:
	uint32_t m_glID = 0;

private:
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

YEASTEM_END
