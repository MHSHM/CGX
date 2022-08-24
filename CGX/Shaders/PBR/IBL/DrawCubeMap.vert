#version 440 core

layout (location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 view_rotation;
uniform mat4 proj;

out vec3 sample_pos;

void main()
{
	sample_pos = in_position;
	gl_Position = proj * view_rotation * vec4(in_position, 1.0f);
}