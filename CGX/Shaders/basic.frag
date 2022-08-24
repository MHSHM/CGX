#version 330 core

uniform sampler2D scene;

in vec2 o_uv; 

out vec4 color;

void main()
{
	color = texture(scene, o_uv);
}