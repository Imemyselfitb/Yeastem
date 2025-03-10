#pragma once

YEASTEM_BEGIN

class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(const void* data, uint32_t size);
	~VertexBuffer();

public:
	void Bind() const;
	void Unbind() const;
	void UpdateBuffer(const void* data) const;
	void UpdateBuffer(const void* data, uint32_t size) const;

private:
	uint32_t m_glID;
	uint32_t m_Size = 0;
};

YEASTEM_END
