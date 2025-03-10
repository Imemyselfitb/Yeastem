#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float texIndex;

out vec2 v_TexCoord;
out float v_TexIndex;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);;
	v_TexCoord = texCoord;
	v_TexIndex = texIndex;
}