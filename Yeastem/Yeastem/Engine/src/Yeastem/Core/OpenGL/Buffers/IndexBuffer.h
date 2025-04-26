#pragma once

YEASTEM_BEGIN

class IndexBuffer
{
public:
	typedef uint32_t Type;

public:
	IndexBuffer();
	~IndexBuffer();

	// count = Number of Indices
	IndexBuffer(const IndexBuffer::Type* data, uint32_t count);

public:
	void Bind() const;
	void Unbind() const; 
	void UpdateBuffer(const void* data) const;
	void UpdateBuffer(const void* data, uint32_t count) const;

private:
	uint32_t m_glID = 0;

public:
	uint32_t size = 0;
};

YEASTEM_END
