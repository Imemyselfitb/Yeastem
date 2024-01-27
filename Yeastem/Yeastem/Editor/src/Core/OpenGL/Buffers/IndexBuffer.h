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
	void UpdateBuffer(const void* data) const;
	void UpdateBuffer(const void* data, unsigned int count);

private:
	unsigned int m_glID;

public:
	unsigned int size;
};

YEASTEM_END
