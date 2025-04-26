#pragma once

YEASTEM_BEGIN

class Shader
{
public:
	Shader() {}
	Shader(const char* VertFile, const char* FragFile) { AssignShaderFromFiles(VertFile, FragFile); }
	~Shader() { DeleteShader(); }

private:
	static uint32_t CompileShader(uint32_t type, const char* source);

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
	int GetUniformLocation(const char*);

	void SetUniform1f(const char*, float);
	void SetUniform1i(const char*, int);
	void SetUniform1iv(const char*, GLsizei count, const int* value);
	void SetUniform4f(const char*, float, float, float, float);
	void SetUniform4i(const char*, int, int, int, int);

private:
	std::unordered_map<const char*, int> m_UniformsCache;
	uint32_t m_Shader = NULL;
};

YEASTEM_END
