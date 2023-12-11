#version 330 core

layout(location = 0) out vec4 glColour;

in vec2 v_TexCoord;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform float u_Time;
uniform int u_ShapeID;

void main()
{
	vec4 texCol0 = texture(u_tex0, v_TexCoord);
	if(u_ShapeID == 1) {
		vec4 texCol1 = texture(u_tex1, v_TexCoord);
		float k = 0.5 * sin(u_Time) + 0.5;
		glColour = texCol0 + k * (texCol1 - texCol0);
		return;
	}

	vec2 texCoord1 = 0.5 - ((0.5 - v_TexCoord) * 1.5);
	vec4 texCol1 = texture(u_tex1, texCoord1);

	if(texCol0.x + texCol0.y + texCol0.z > 1.0) {
		glColour = texCol0;
	} else {
		glColour = texCol1;
	}
}