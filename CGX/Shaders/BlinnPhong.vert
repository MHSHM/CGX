#version 330 core

layout (location = 0) in vec3 in_position; 
layout (location = 1) in vec3 in_normal; 
layout (location = 2) in vec2 uv;

uniform mat4 mvp;
uniform mat4 model;

out vec3 o_normal;
out vec3 o_position;
out vec2 o_uv;

void main()
{
	gl_Position = mvp * vec4(in_position, 1.0f);
	
	o_normal = normalize(vec3(model * vec4(in_normal, 0.0f)));
	o_position = vec3(model * vec4(in_position, 1.0f));
	o_uv = uv;
}