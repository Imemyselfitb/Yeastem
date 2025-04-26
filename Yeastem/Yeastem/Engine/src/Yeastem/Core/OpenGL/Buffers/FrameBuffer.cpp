#include "yst_pch.h"

#include "FrameBuffer.h"

YEASTEM_BEGIN

void FrameBuffer::Create(uint32_t width, uint32_t height)
{
	glGenFramebuffers(1, &m_glID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_glID);

	glGenTextures(1, &m_glTextureID);
	glBindTexture(GL_TEXTURE_2D, m_glTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glTextureID, 0);

	glGenRenderbuffers(1, &m_glRenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_glRenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_glRenderBufferID);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (Status)
		{
		case 36057u:
			YEASTEM_ERROR("Frame Buffer Creation: Not all Attached Images have the same Width and Height! ");
			break;
		case 36055u:
			YEASTEM_ERROR("Frame Buffer Creation: No Images attached to the Frame Buffer! ");
			break;
		case 36061u:
			YEASTEM_ERROR("Frame Buffer Creation: Internal Formats of Images Violate Implementation-Dependant Restrictioins! ");
			break;
		case 26054u:
			YEASTEM_ERROR("Frame Buffer Creation: At least one Attachment Point has its Attached Object no Longer in Existance! ");
			break;
		}
	}
}

void FrameBuffer::Destroy() const
{
	glDeleteFramebuffers(1, &m_glID);
	glDeleteTextures(1, &m_glTextureID);
	glDeleteRenderbuffers(1, &m_glRenderBufferID);
}

FrameBuffer::~FrameBuffer()
{
	Destroy();
}

void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_glID);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error!\n";
	}
}

void FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
YEASTEM_END
