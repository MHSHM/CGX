#version 440 core 

uniform sampler2D image;

in vec2 o_uv; 

out vec4 color; 

void main()
{
	color = texture(image, o_uv);
}