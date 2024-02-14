#pragma once

YEASTEM_BEGIN

class FrameBuffer
{
public:
	FrameBuffer(unsigned int width, unsigned int height) { this->Create(width, height); }
	~FrameBuffer();

	void Create(unsigned int width, unsigned int height);
	void Destroy();

	unsigned int GetColourAttachmentID() const { return this->m_glTextureID; }
	unsigned int GetDepthAttachmentID() const { return this->m_glRenderBufferID; }

	void Bind();
	void Unbind();

private:
	unsigned int m_glID;
	unsigned int m_glTextureID;
	unsigned int m_glRenderBufferID;
};

YEASTEM_END
