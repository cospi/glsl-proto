#version 400 core

uniform mat4 u_transform;
uniform float u_time;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

out vec2 v_uv;

void main()
{
	gl_Position = u_transform * vec4(a_position, 1.0);
	v_uv = a_uv;
}
