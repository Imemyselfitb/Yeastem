#pragma once

YEASTEM_BEGIN

class Shader
{
public:
	Shader() {}
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

private:
	unsigned int m_Shader = NULL;
};

YEASTEM_END
