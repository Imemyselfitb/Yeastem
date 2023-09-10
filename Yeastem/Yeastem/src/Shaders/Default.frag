#version 330 core

layout(location = 0) out vec4 glColour;

void main()
{
	glColour = vec4(1.0, gl_FragCoord.y / 480.0, 0.0, 1.0);
}