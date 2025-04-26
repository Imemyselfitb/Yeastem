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

	template<typename T> void Push(uint32_t count) {
		YEASTEM_ERROR("UNDEFINED VERTEX ARRAY TYPE!");
		static_assert(false, "UNDEFINED VERTEX ARRAY TYPE!");
	}

	template<> void Push<float>(uint32_t count) { PushType(count, GL_FLOAT, GL_FALSE); }
	template<> void Push<int>(uint32_t count) { PushType(count, GL_INT, GL_TRUE); }
	template<> void Push<short>(uint32_t count) { PushType(count, GL_SHORT, GL_TRUE); }
	template<> void Push<char>(uint32_t count) { PushType(count, GL_BYTE, GL_TRUE); }
	template<> void Push<uint32_t>(uint32_t count) { PushType(count, GL_UNSIGNED_INT, GL_TRUE); }
	template<> void Push<uint16_t>(uint32_t count) { PushType(count, GL_UNSIGNED_SHORT, GL_TRUE); }
	template<> void Push<uint8_t>(uint32_t count) { PushType(count, GL_UNSIGNED_BYTE, GL_TRUE); }

	inline const Format& getElements() const { return m_Layout; }

	inline uint32_t getStride() const { return m_Stride; }

private:
	void PushType(uint32_t count, uint32_t glType, uint8_t normalised)
	{ m_Layout.push_back({ glType, count, normalised }); m_Stride += count * GetSizeOfGLType(glType); }

private:
	std::vector<VertexBufferLayoutElement> m_Layout;
	uint32_t m_Stride = 0;
};


YEASTEM_END
