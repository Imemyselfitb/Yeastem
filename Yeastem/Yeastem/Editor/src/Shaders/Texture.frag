#version 330 core

layout(location = 0) out vec4 glColour;

in vec2 v_TexCoord;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform float u_Time;

void main()
{
	vec4 texCol0 = texture(u_tex0, v_TexCoord);
	vec4 texCol1 = texture(u_tex1, v_TexCoord);
	
	float k = 0.5 * sin(u_Time) + 0.5;
	vec4 col = texCol0 + k * (texCol1 - texCol0);

	glColour = col;
}