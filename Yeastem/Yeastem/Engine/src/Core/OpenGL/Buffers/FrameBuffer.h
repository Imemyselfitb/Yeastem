#pragma once

YEASTEM_BEGIN

class FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height) { Create(width, height); }
	~FrameBuffer();

	void Create(uint32_t width, uint32_t height);
	void Destroy() const;

	uint32_t GetColourAttachmentID() const { return m_glTextureID; }
	uint32_t GetDepthAttachmentID() const { return m_glRenderBufferID; }

	void Bind() const;
	void Unbind() const;

private:
	uint32_t m_glID;
	uint32_t m_glTextureID;
	uint32_t m_glRenderBufferID;
};

YEASTEM_END
