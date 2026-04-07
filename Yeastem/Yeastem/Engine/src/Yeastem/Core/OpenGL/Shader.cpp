#include "yst_pch.h"

#include "Shader.h"

#include "Yeastem/FileIO/FileIO.h"

#include <glm/gtc/type_ptr.hpp>

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

	return program;
}
uint32_t Shader::CreateShaderFromFiles(const char* vert, const char* frag)
{
	return CreateShader(
		FileIO::readFileText(vert).c_str(),
		FileIO::readFileText(frag).c_str()
	);
}

void Shader::InitiateUniformBindings()
{
	m_UniformBindings[ShaderParams::ViewProjectionMatrix] = GetUniformLocation("u_ViewProjectionMatrix");
	m_UniformBindings[ShaderParams::Textures] = GetUniformLocation("u_Textures");
}

void Shader::AssignShader(const char* vertexShader, const char* fragmentShader)
{
	DeleteShader();
	m_Shader = CreateShader(vertexShader, fragmentShader);
	InitiateUniformBindings();
}
void Shader::AssignShaderFromFiles(const char* vertexShader, const char* fragmentShader)
{
	DeleteShader();
	m_Shader = CreateShaderFromFiles(vertexShader, fragmentShader);
	InitiateUniformBindings();
}

GLint Shader::GetUniformLocation(std::string_view name) const
{
	Bind();
	GLint location = glGetUniformLocation(m_Shader, name.data());
	if (location == -1)
		YEASTEM_WARNING("Uniform `" << name << "` does not exist! ");

	return { location };
}

static const char* uniformNames[] = {
	"u_ViewProjectionMatrix",
	"u_Textures",
};

#define getUniformBinding(uniform) \
	if (m_UniformBindings.find(uniform) == m_UniformBindings.end()) { YEASTEM_ERROR("Uniform `" << uniformNames[uniform] << "` binding does not exist!"); return; } \
	Bind(); GLint location = m_UniformBindings.at(uniform);

void Shader::SetUniformInt(ShaderParams uniform, int v0) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniform1i(location, v0);
}
void Shader::SetUniformIntArray(ShaderParams uniform, GLsizei count, const int* value) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniform1iv(location, count, value);
}
void Shader::SetUniformFloat(ShaderParams uniform, float v0) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniform1f(location, v0);
}
void Shader::SetUniformFloat2(ShaderParams uniform, const glm::vec2& value) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniform2f(location, value.x, value.y);
}
void Shader::SetUniformFloat3(ShaderParams uniform, const glm::vec3& value) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniform3f(location, value.x, value.y, value.z);
}
void Shader::SetUniformFloat4(ShaderParams uniform, const glm::vec4& value) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Shader::SetUniformMat4(ShaderParams uniform, const glm::mat4& mat4) const
{
	getUniformBinding(uniform);
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
}

YEASTEM_END
