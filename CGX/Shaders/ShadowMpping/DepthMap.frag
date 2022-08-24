#version 330 core

// world space
uniform vec3 light_pos;

in vec4 o_frag_pos; 

out vec4 color;

void main()
{
	float dist_frag_to_light = length(light_pos - vec3(o_frag_pos));

	dist_frag_to_light = dist_frag_to_light / 25.0f;

	gl_FragDepth = dist_frag_to_light;
}