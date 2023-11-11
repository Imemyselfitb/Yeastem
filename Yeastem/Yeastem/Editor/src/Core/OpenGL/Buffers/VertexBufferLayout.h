#pragma once

YEASTEM_BEGIN

unsigned int GetSizeOfGLType(unsigned int type);

struct VertexBufferLayoutElement
{
	unsigned int type; // GL_INT, GL_FLOAT, etc.
	unsigned int count; // Count of attributes between vertices.
	unsigned char normalized;

	unsigned int openGlTypeSize = GetSizeOfGLType(type);
};

class VertexBufferLayout
{
public:
	using Format = std::vector<VertexBufferLayoutElement>;

public:
	VertexBufferLayout() {}
	~VertexBufferLayout() {}

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Layout.push_back({ GL_FLOAT, count, GL_FALSE });
		this->m_Stride += count * GetSizeOfGLType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Layout.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		this->m_Stride += count * GetSizeOfGLType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Layout.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		this->m_Stride += count * GetSizeOfGLType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferLayoutElement>& \
		getElements() const { return this->m_Layout; }

	inline unsigned int getStride() const
	{ return this->m_Stride; }

private:
	std::vector<VertexBufferLayoutElement> m_Layout;
	unsigned int m_Stride = 0;
};


YEASTEM_END
