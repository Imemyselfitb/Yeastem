#pragma once

YEASTEM_BEGIN

class IndexBuffer
{
public:
	// count = Number of Indices
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

public:
	void Bind() const;
	void Unbind() const;

private:
	unsigned int m_glID;
	unsigned int m_Count;
};

YEASTEM_END
