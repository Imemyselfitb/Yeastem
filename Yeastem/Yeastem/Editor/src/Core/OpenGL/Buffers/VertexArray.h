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

	void Bind();
	void Unbind();

public:
	unsigned int m_glID = 0;
};

YEASTEM_END
