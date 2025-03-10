#pragma once

YEASTEM_BEGIN

uint32_t GetSizeOfGLType(uint32_t type);

struct VertexBufferLayoutElement
{
	uint32_t type; // GL_INT, GL_FLOAT, etc.
	uint32_t count; // Count of attributes between vertices.
	unsigned char normalized;

	uint32_t openGlTypeSize = GetSizeOfGLType(type);
};

class VertexBufferLayout
{
public:
	using Format = std::vector<VertexBufferLayoutElement>;

public:
	VertexBufferLayout() {}
	~VertexBufferLayout() {}

	template<typename T>
	void Push(uint32_t count)
	{}

	template<>
	void Push<float>(uint32_t count)
	{
		m_Layout.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * GetSizeOfGLType(GL_FLOAT);
	}

	template<>
	void Push<uint32_t>(uint32_t count)
	{
		m_Layout.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * GetSizeOfGLType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(uint32_t count)
	{
		m_Layout.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * GetSizeOfGLType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferLayoutElement>& \
		getElements() const { return m_Layout; }

	inline uint32_t getStride() const
	{ return m_Stride; }

private:
	std::vector<VertexBufferLayoutElement> m_Layout;
	uint32_t m_Stride = 0;
};


YEASTEM_END
