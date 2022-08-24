#version 440 core

layout (location = 0) in vec3 in_position; 
layout (location = 1) in vec3 in_normal; 
layout (location = 2) in vec2 in_uv; 
layout (location = 3) in vec3 tangent; 
layout (location = 4) in vec3 bitangent; 

out vec2 o_uv; 

void main()
{
	gl_Position = vec4(in_position, 1.0f);
	o_uv = in_uv; 
}