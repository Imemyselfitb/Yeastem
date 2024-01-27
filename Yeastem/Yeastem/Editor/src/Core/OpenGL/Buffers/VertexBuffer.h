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
	void UpdateBuffer(const void* data) const;
	void UpdateBuffer(const void* data, unsigned int size);

private:
	unsigned int m_glID;
	unsigned int m_Size;
};

YEASTEM_END
