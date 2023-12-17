#pragma once

YEASTEM_BEGIN

class Shader
{
public:
	Shader() {}
	Shader(const std::string& VertFile, const std::string& FragFile) { this->AssignShaderFromFiles(VertFile, FragFile); }
	~Shader() { this->DeleteShader(); }

private:
	static unsigned int CompileShader(unsigned int type, const std::string& source);

public:
	static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	static unsigned int CreateShaderFromFiles(const std::string& vert, const std::string& frag);

public:
	void AssignShader(const std::string& vertexShader, const std::string& fragmentShader);
	void AssignShaderFromFiles(const std::string& vert, const std::string& frag);

	void DeleteShader();

	void Bind() const;
	void Unbind() const;

	unsigned int get() const { return this->m_Shader; }
	operator unsigned int() const { return this->m_Shader; }

public:
	int GetUniformLocation(const std::string&);

	void SetUniform1f(const std::string&, float);
	void SetUniform1i(const std::string&, int);
	void SetUniform4f(const std::string&, float, float, float, float);
	void SetUniform4i(const std::string&, int, int, int, int);

private:
	std::unordered_map<std::string, int> m_UniformsCache;
	unsigned int m_Shader = NULL;
};

YEASTEM_END
