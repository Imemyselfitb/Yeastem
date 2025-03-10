#version 330 core

layout(location = 0) out vec4 glColour;

in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_textures[16];
uniform float u_Time;

void main()
{
	vec4 col = texture(u_textures[int(v_TexIndex)], v_TexCoord);
	glColour = col;
}