#pragma once

YEASTEM_BEGIN

class Texture
{
public:
	Texture() {}
	Texture(const std::filesystem::path& path) {
		LoadTexture(path);
	}

	~Texture();

	void LoadTexture(const std::filesystem::path& path);

	void Bind(uint32_t slot = 0U);
	void Unbind();

	inline uint32_t getWidth() const { return m_Width; }
	inline uint32_t getHeight() const { return m_Height; }
	inline uint32_t getOpenGlID() const { return m_glID; }
	inline bool isInitialised() const { return m_Initialized; }
	inline const std::filesystem::path& getFilePath() const { return m_FilePath; }

	inline bool isBound() const { return m_BoundSlot > 0; }
	inline uint32_t getBoundSlot() const { return m_BoundSlot - 1; }

private:
	void SetupGLFlags();
	static void InitSettings();

private:
	uint32_t m_glID = 0;
	uint8_t m_BoundSlot = 0;

	bool m_Initialized = false;
	std::filesystem::path m_FilePath;
	int m_Width = 0, m_Height = 0, m_BPP = 0;

private:
	inline static bool s_InitialisedSettings;
	static uint32_t s_TextureCount;
};

YEASTEM_END
