#version 400 core

uniform sampler2D u_texture;
uniform float u_time;

in vec2 v_uv;

out vec4 o_color;

void main()
{
	o_color = texture(u_texture, v_uv);
}
