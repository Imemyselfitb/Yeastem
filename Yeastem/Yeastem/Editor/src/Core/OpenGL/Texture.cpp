#include "yst_pch.h"

#include "Texture.h"

YEASTEM_BEGIN

void Texture::LoadTexture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);

	stbi_uc* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	if (!data)
	{
		YEASTEM_ERROR("Texture `" << path << "` could not load! ");
		return;
	}

	this->m_Initialized = true;
	this->m_FilePath = path;

	glGenTextures(1, &this->m_glID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->m_glID);

	this->SetupGLFlags();

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8, 
		m_Width, m_Height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, 
		data
	);

	stbi_image_free(data);

	std::cout << "Yestem: Graphics: Texture `" << path << "` Created!" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetupGLFlags()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void Texture::Bind(unsigned int slot) 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->m_glID);
	this->m_CurrentSlot = slot;
}

void Texture::Unbind()
{
	glActiveTexture(GL_TEXTURE0 + this->m_CurrentSlot);
	glBindTexture(GL_TEXTURE_2D, 0);

	this->m_CurrentSlot = 0;
}

Texture::Texture(const Texture& other)
{
	if (!other.m_Initialized) return; 
	std::cout << "!Texture `" << other.m_glID << "` Copied!" << std::endl;
}

Texture::~Texture()
{
	std::cout << "Yestem: Graphics: Texture `" << this->m_FilePath << "` Deleted!" << std::endl;
	glDeleteTextures(1, &this->m_glID);
}

YEASTEM_END
