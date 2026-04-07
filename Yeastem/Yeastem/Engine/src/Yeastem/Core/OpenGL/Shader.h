#pragma once

YEASTEM_BEGIN

enum ShaderParams {
	// Vertex
	ViewProjectionMatrix = 0,

	// Fragment
	Textures,
};

class Shader
{
public:
	Shader() {}
	Shader(const char* VertFile, const char* FragFile) { AssignShader(VertFile, FragFile); }
	Shader(const std::filesystem::path& VertFile, const std::filesystem::path& FragFile)
	{ AssignShaderFromFiles(VertFile.string().c_str(), FragFile.string().c_str()); }
	~Shader() { DeleteShader(); }

public:
	static uint32_t CreateShader(const char* vertexShader, const char* fragmentShader);
	static uint32_t CreateShaderFromFiles(const char* vert, const char* frag);

public:
	void AssignShader(const char* vertexShader, const char* fragmentShader);
	void AssignShaderFromFiles(const char* vert, const char* frag);

	void DeleteShader() const;

	void Bind() const;
	void Unbind() const;

	uint32_t get() const { return m_Shader; }
	operator uint32_t() const { return m_Shader; }

public:
	void SetUniformInt(ShaderParams, int value) const;
	void SetUniformIntArray(ShaderParams, GLsizei count, const int* values) const;
	void SetUniformFloat(ShaderParams, float value) const;
	void SetUniformFloat2(ShaderParams, const glm::vec2& value) const;
	void SetUniformFloat3(ShaderParams, const glm::vec3& value) const;
	void SetUniformFloat4(ShaderParams, const glm::vec4& value) const;
	void SetUniformMat4(ShaderParams, const glm::mat4& value) const;

private:
	static uint32_t CompileShader(uint32_t type, const char* source);

private:
	GLint GetUniformLocation(std::string_view) const;
	void InitiateUniformBindings();

private:
	std::unordered_map<ShaderParams, GLint> m_UniformBindings;
	uint32_t m_Shader = NULL;
};

YEASTEM_END
