#include "yst_pch.h"

#include "ShaderContainer.h"

YEASTEM_BEGIN

bool ShaderSourceFiles::operator==(const ShaderSourceFiles& other) const
{
	return (VertexShader == other.VertexShader) && (FragmentShader == other.FragmentShader);
}

ShaderContainer ShaderContainer::s_Instance;
void ShaderContainer::AddShader(ShaderSourceFiles src, std::shared_ptr<Shader> shader)
{
	s_Instance.shaders[src] = shader;
}

std::shared_ptr<Shader> ShaderContainer::GetShader(ShaderSourceFiles src)
{
	if (s_Instance.shaders.find(src) == s_Instance.shaders.end())
		return nullptr;

	return s_Instance.shaders.at(src);
}

YEASTEM_END
