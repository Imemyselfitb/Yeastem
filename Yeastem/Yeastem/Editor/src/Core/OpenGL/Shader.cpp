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
	std::cout << "Shader Deleted!" << std::endl;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	// Create Shader
	unsigned int id = glCreateShader(type);

	// Compile the Source Code inside the Shader
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Error Handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		// Get Error Length to...
		int err_length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &err_length);

		// Stack allocate a buffer of memeory, where the message will live
		char* info = (char*)alloca(err_length * sizeof(char));
		glGetShaderInfoLog(id, err_length, &err_length, info);

		// Print out the message using <iostream>
		std::cout << "ERROR: Error Compiling " << (type == GL_VERTEX_SHADER ? "Vertex " : "Fragment ");
		std::cout << "Shader! \n" << info << std::endl;

		// Delete Shader and Exit
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	std::cout << "Shader Created!" << std::endl;

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

YEASTEM_END
