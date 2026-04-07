#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float texIndex;
layout(location = 2) in vec4 entityID;

uniform mat4 u_ViewProjectionMatrix;

out vec2 v_TexCoord;
out float v_TexIndex;
out vec4 v_EntityID;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(position, 1.0);
	v_TexCoord = texCoord;
	v_TexIndex = texIndex;
}