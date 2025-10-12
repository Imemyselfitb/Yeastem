#pragma once

YEASTEM_BEGIN

uint32_t GetSizeOfGLType(uint32_t type);

struct VertexBufferLayoutElement
{
	uint32_t type; // GL_INT, GL_FLOAT, etc.
	uint32_t count; // Count of attributes between vertices.
	uint8_t normalized;

	uint32_t openGlTypeSize = GetSizeOfGLType(type);
};

class VertexBufferLayout
{
public:
	using Format = std::vector<VertexBufferLayoutElement>;

public:
	VertexBufferLayout() {}
	~VertexBufferLayout() {}

	template<typename T> void Push(uint32_t count);

	inline const Format& getElements() const { return m_Layout; }

	inline uint32_t getStride() const { return m_Stride; }

private:
	void PushType(uint32_t count, uint32_t glType, uint8_t normalised);

private:
	std::vector<VertexBufferLayoutElement> m_Layout;
	uint32_t m_Stride = 0;
};


YEASTEM_END
