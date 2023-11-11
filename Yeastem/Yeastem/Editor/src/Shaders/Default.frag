#version 330 core

layout(location = 0) out vec4 glColour;

in vec2 v_TexCoord;

uniform float u_Time;

void main()
{
	float r = 0.5f * sin(u_Time) + 0.5f;
	float g = 1.0f - r;
	float b = sin(r * 3.1415926535897932384626433832795);
	vec4 col = vec4(r, g, b, 1.0f);

	glColour = col;
}