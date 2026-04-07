#version 330 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out vec4 outEntityID;

in vec2 v_TexCoord;
in float v_TexIndex;
in vec4 v_EntityID;

uniform sampler2D u_Textures[16];
uniform float u_Time;

void main()
{
	vec4 col = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
	outColour = col;
}