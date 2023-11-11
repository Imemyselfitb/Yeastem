#pragma once

YEASTEM_BEGIN

class Texture
{
public:
	Texture() {}
	Texture(const Texture&);
	Texture(const std::string& path) { 
		this->LoadTexture(path);
	}

	~Texture();

	void LoadTexture(const std::string& path);

	// Note: Can't be marked `const`
	void Bind(unsigned int slot = 0U);
	void Unbind();

	inline unsigned int getWidth() const { return this->m_Width; }
	inline unsigned int getHeight() const { return this->m_Height; }
	inline unsigned int getOpenGL_ID() { return this->m_glID; }
	inline unsigned int getBoundSlot() const { return this->m_CurrentSlot; }
	inline bool isInitialised() const { return this->m_Initialized; }
	inline const std::string& getFileName() const
	{ return this->m_FilePath; }

private:
	void SetupGLFlags();

private:
	unsigned int m_glID;
	
	bool m_Initialized = false;
	std::string m_FilePath;
	int m_Width, m_Height, m_BPP;
	unsigned int m_CurrentSlot = 0;
};

YEASTEM_END
