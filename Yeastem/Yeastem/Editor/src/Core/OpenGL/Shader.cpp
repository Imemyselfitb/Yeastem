#include "yst_pch.h"

#include "Shader.h"

#include "FileIO/FileIO.h"

YEASTEM_BEGIN

void Shader::Bind() const
{
	glUseProgram(this->m_Shader);
}
void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::DeleteShader()
{
	if (this->m_Shader == NULL) return;
	glDeleteShader(this->m_Shader);
	std::cout << "Yestem: Graphics: Shader Deleted!" << std::endl;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	// Create Shader
	unsigned int id = glCreateShader(type);

	// Compile the Source Code inside the Shader
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, 0);

	glCompileShader(id);

	// Error Handling
	int result = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		// Get Error Length to...
		int err_length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &err_length);

		// Stack allocate a buffer of memeory, where the message will live
		std::vector<GLchar> infoLog(err_length);
		glGetShaderInfoLog(id, err_length, &err_length, &infoLog[0]);

		// Print out the message using <iostream>
		std::cout << "ERROR: Error Compiling " << (type == GL_VERTEX_SHADER ? "Vertex " : "Fragment ");
		std::cout << "Shader! \n" << *(&infoLog[0]) << std::endl;

		// Delete Shader and Exit
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	std::cout << "Yestem: Graphics: Shader Created!" << std::endl;

	// Create program and Vertex + Fragment Shaders
	unsigned int program = glCreateProgram();
	unsigned int vert = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int frag = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Attach the Shaders and Link the program. 
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glValidateProgram(program);

	// Delete the 'intermediate' Shaders
	glDeleteShader(vert);
	glDeleteShader(frag);

	return program;
}
unsigned int Shader::CreateShaderFromFiles(const std::string& vert, const std::string& frag)
{
	return CreateShader(
		readFileText(vert),
		readFileText(frag)
	);
}

void Shader::AssignShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	this->DeleteShader();
	this->m_Shader = CreateShader(vertexShader, fragmentShader);
}
void Shader::AssignShaderFromFiles(const std::string& vertexShader, const std::string& fragmentShader)
{
	this->DeleteShader();
	this->m_Shader = CreateShaderFromFiles(vertexShader, fragmentShader);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (this->m_UniformsCache.find(name) != this->m_UniformsCache.end())
		return this->m_UniformsCache[name];
	
	int location = glGetUniformLocation(this->m_Shader, name.c_str());
	if (location == -1)
	{
		YEASTEM_WARNING("uniform `" + name + "` does not exist! ");
	}

	m_UniformsCache[name] = location;
	return location;
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	int loc = this->GetUniformLocation(name);
	if (loc != -1) glUniform1f(loc, v0);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
	int loc = this->GetUniformLocation(name);
	if (loc != -1) glUniform1i(loc, v0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	int loc = this->GetUniformLocation(name);
	if (loc != -1) glUniform4f(loc, v0, v1, v2, v3);
}

void Shader::SetUniform4i(const std::string& name, int v0, int v1, int v2, int v3)
{
	int loc = this->GetUniformLocation(name);
	if (loc != -1) glUniform4i(loc, v0, v1, v2, v3);
}

YEASTEM_END
