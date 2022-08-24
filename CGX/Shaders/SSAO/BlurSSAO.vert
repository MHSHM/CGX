#version 440 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;

out vec2 o_uv;

void main()
{
	gl_Position = vec4(in_position, 1.0f);
	o_uv = in_uv; 
}