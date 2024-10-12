#version 400 core

uniform mat4 u_projection;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec4 a_color;

out vec4 v_color;

void main()
{
	gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
	v_color = a_color;
}
