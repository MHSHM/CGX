#version 440 core

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 diffuse; 
layout (location = 3) out vec4 specular; 

uniform sampler2D normal_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

in vec4 o_position; 
in mat3 o_TBN; 
in vec2 o_uv; 
in vec3 o_normal; 

void main()
{
	vec3 mapped_normal = texture(normal_map, o_uv).rgb;
	mapped_normal = mapped_normal * 2.0f - 1.0f;

	position = o_position;
	normal = vec4(normalize(o_normal), 1.0f);
	diffuse = texture(diffuse_map, o_uv);
	specular = texture(specular_map, o_uv);
}