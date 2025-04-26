#include "yst_pch.h"

#include "Texture.h"

#include <stb_image.h>

YEASTEM_BEGIN

void Texture::LoadTexture(const std::string& path)
{
	if (!s_InitialisedSettings)
		InitSettings();

	// stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);

	if (!data)
	{
		YEASTEM_ERROR("Texture `" << path << "` could not load! - " << stbi_failure_reason());
		return;
	}

	GLenum InternalOpenGLFormat, ImageDataFormat;
	switch (m_BPP)
	{
	case 4:
		InternalOpenGLFormat = GL_RGBA8;
		ImageDataFormat = GL_RGBA;
		break;
	case 3:
		InternalOpenGLFormat = GL_RGB8;
		ImageDataFormat = GL_RGB;
		break;
	case 1:
		InternalOpenGLFormat = GL_R8;
		ImageDataFormat = GL_R;
		break;
	default:
		YEASTEM_ERROR("Texture Format For `" << path << "` Not Supported!");
		return;
	}

	m_Initialized = true;
	m_FilePath = path;

	glGenTextures(1, &m_glID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_glID);

	SetupGLFlags();

	glTexImage2D(
		GL_TEXTURE_2D, 0, InternalOpenGLFormat,
		m_Width, m_Height, 0, 
		ImageDataFormat, GL_UNSIGNED_BYTE,
		data
	);

	stbi_image_free(data);

	std::cout << "Yestem: Graphics: Texture `" << path << "` Created! " << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::InitSettings()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	s_InitialisedSettings = true;
}

void Texture::SetupGLFlags()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::Bind(uint32_t slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_glID);
	m_BoundSlot = slot + 1;
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	m_BoundSlot = 0;
}

Texture::~Texture()
{
	std::cout << "Yestem: Graphics: Texture `" << m_FilePath << "` Deleted!" << std::endl;
	glDeleteTextures(1, &m_glID);
}

YEASTEM_END
