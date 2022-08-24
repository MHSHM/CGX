#version 330 core

uniform samplerCube env;

in vec3 sample_pos;

out vec4 color;

void main()
{
	color = texture(env, sample_pos * vec3(1.0f, -1.0f, 1.0f));
}
