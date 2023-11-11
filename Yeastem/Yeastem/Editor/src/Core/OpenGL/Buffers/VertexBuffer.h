#pragma once

YEASTEM_BEGIN

class VertexBuffer
{
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

public:
	void Bind() const;
	void Unbind() const;

private:
	unsigned int m_glID;
};

YEASTEM_END
