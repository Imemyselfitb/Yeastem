#pragma once

#include "Shader.h"

YEASTEM_BEGIN

struct ShaderSourceFiles
{
	std::string VertexShader;
	std::string FragmentShader;

	bool operator==(const ShaderSourceFiles& other) const;
};

YEASTEM_END

template<>
struct std::hash<Yeastem::ShaderSourceFiles>
{
	size_t operator()(const Yeastem::ShaderSourceFiles& src) const
	{
		auto a = std::hash<std::string>()(src.VertexShader);
		auto b = std::hash<std::string>()(src.VertexShader);

		return (a * 19981091998181U) + b;
	}
};

YEASTEM_BEGIN

class ShaderContainer
{
public:
	static void AddShader(ShaderSourceFiles src, std::shared_ptr<Shader> shader);
	static std::shared_ptr<Shader> GetShader(ShaderSourceFiles src);

private:
	ShaderContainer() {}
	ShaderContainer(const ShaderContainer&) = delete;
	~ShaderContainer() {}

private:
	std::unordered_map<ShaderSourceFiles, std::shared_ptr<Shader>> shaders;

private:
	static ShaderContainer s_Instance;
};

YEASTEM_END
