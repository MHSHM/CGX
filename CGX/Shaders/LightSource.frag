#version 330 core

const float MAX_INTENSITY = 100.0f;

uniform vec3 light_intensity;

out vec4 color;

void main()
{
	color = vec4(light_intensity / vec3(MAX_INTENSITY), 1.0f);
}