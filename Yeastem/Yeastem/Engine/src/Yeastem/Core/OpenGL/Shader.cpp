#include "yst_pch.h"

#include "Shader.h"

#include "Yeastem/FileIO/FileIO.h"

YEASTEM_BEGIN

void Shader::Bind() const
{
	glUseProgram(m_Shader);
}
void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::DeleteShader() const
{
	if (m_Shader == NULL) return;
	glDeleteShader(m_Shader);
	std::cout << "Yestem: Graphics: Shader Deleted!" << std::endl;
}

uint32_t Shader::CompileShader(uint32_t type, const char* source)
{
	// Create Shader
	uint32_t id = glCreateShader(type);

	// Compile the Source Code inside the Shader
	glShaderSource(id, 1, &source, 0);

	glCompileShader(id);

	// Error Handling
	int result = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		// Get Error Length to...
		int err_length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &err_length);

		if (err_length > 0)
		{
			GLchar* infoLog = new GLchar [err_length];
			glGetShaderInfoLog(id, err_length, &err_length, infoLog);

			// Print out the message using <iostream>
			YEASTEM_ERROR("Error Compiling " << (type == GL_VERTEX_SHADER ? "QuadVertex " : "Fragment ") << "Shader! \nCAUSE:"
				<< std::string(&infoLog[0]));

			// Delete Shader and Exit
			glDeleteShader(id);
			return 0;
		}
		else
		{
			return id;
		}
	}

	return id;
}

uint32_t Shader::CreateShader(const char* vertexShader, const char* fragmentShader)
{
	std::cout << "Yestem: Graphics: Shader Created!" << std::endl;

	// Create program and QuadVertex + Fragment Shaders
	uint32_t program = glCreateProgram();
	uint32_t vert = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Attach the Shaders and Link the program. 
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glValidateProgram(program);

	// Delete the 'intermediate' Shaders
	glDeleteShader(vert);
	glDeleteShader(frag);

	glUseProgram(0);

	return program;
}
uint32_t Shader::CreateShaderFromFiles(const char* vert, const char* frag)
{
	return CreateShader(
		FileIO::readFileText(vert).c_str(),
		FileIO::readFileText(frag).c_str()
	);
}

void Shader::AssignShader(const char* vertexShader, const char* fragmentShader)
{
	DeleteShader();
	m_Shader = CreateShader(vertexShader, fragmentShader);
}
void Shader::AssignShaderFromFiles(const char* vertexShader, const char* fragmentShader)
{
	DeleteShader();
	m_Shader = CreateShaderFromFiles(vertexShader, fragmentShader);
}

int Shader::GetUniformLocation(const char* name)
{
	if (m_UniformsCache.find(name) != m_UniformsCache.end())
		return m_UniformsCache[name];
	
	int location = glGetUniformLocation(m_Shader, name);
	if (location == -1)
	{
		YEASTEM_WARNING("uniform `" << name << "` does not exist! ");
	}

	m_UniformsCache[name] = location;
	return location;
}

void Shader::SetUniform1f(const char* name, float v0)
{
	Bind();
	int loc = GetUniformLocation(name);
	if (loc != -1) glUniform1f(loc, v0);
	Unbind();
}

void Shader::SetUniform1i(const char* name, int v0)
{
	Bind();
	int loc = GetUniformLocation(name);
	if (loc != -1) glUniform1i(loc, v0);
	Unbind();
}

void Shader::SetUniform1iv(const char* name, GLsizei count, const int* value)
{
	Bind();
	int loc = GetUniformLocation(name);
	if (loc != -1) glUniform1iv(loc, count, value);
	Unbind();
}

void Shader::SetUniform4f(const char* name, float v0, float v1, float v2, float v3)
{
	Bind();
	int loc = GetUniformLocation(name);
	if (loc != -1) glUniform4f(loc, v0, v1, v2, v3);
	Unbind();
}

void Shader::SetUniform4i(const char* name, int v0, int v1, int v2, int v3)
{
	Bind();
	int loc = GetUniformLocation(name);
	if (loc != -1) glUniform4i(loc, v0, v1, v2, v3);
	Unbind();
}

YEASTEM_END
