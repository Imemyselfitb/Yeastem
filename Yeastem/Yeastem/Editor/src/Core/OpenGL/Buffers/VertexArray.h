#pragma once

#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

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

public:
	unsigned int m_glID = 0;
};

YEASTEM_END
