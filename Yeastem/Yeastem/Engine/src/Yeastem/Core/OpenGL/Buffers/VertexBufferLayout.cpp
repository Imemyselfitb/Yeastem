#include "yst_pch.h"

#include "VertexBufferLayout.h"

YEASTEM_BEGIN

template<typename>
struct always_false : std::false_type {};

template<typename T> void VertexBufferLayout::Push(uint32_t count) {
	YEASTEM_ERROR("UNDEFINED VERTEX ARRAY TYPE!");
	static_assert(always_false<T>::value, "UNDEFINED VERTEX ARRAY TYPE!");
}
template<> void VertexBufferLayout::Push<float>(uint32_t count) { PushType(count, GL_FLOAT, GL_FALSE); }
template<> void VertexBufferLayout::Push<int>(uint32_t count) { PushType(count, GL_INT, GL_TRUE); }
template<> void VertexBufferLayout::Push<short>(uint32_t count) { PushType(count, GL_SHORT, GL_TRUE); }
template<> void VertexBufferLayout::Push<char>(uint32_t count) { PushType(count, GL_BYTE, GL_TRUE); }
template<> void VertexBufferLayout::Push<uint32_t>(uint32_t count) { PushType(count, GL_UNSIGNED_INT, GL_TRUE); }
template<> void VertexBufferLayout::Push<uint16_t>(uint32_t count) { PushType(count, GL_UNSIGNED_SHORT, GL_TRUE); }
template<> void VertexBufferLayout::Push<uint8_t>(uint32_t count) { PushType(count, GL_UNSIGNED_BYTE, GL_TRUE); }

uint32_t GetSizeOfGLType(uint32_t type)
{
	switch (type)
	{
	case GL_FLOAT: return 4;
	case GL_UNSIGNED_INT: return 4;
	case GL_UNSIGNED_SHORT: return 2;
	case GL_UNSIGNED_BYTE: return 1;
	case GL_INT: return 4;
	case GL_SHORT: return 2;
	case GL_BYTE: return 1;
	};

	YEASTEM_WARNING("OpenGL Type " << type << " not supported. ");
	return 0;
}

void VertexBufferLayout::PushType(uint32_t count, uint32_t glType, uint8_t normalised)
{
	m_Layout.push_back({ glType, count, normalised });
	m_Stride += count * GetSizeOfGLType(glType);
}

YEASTEM_END
