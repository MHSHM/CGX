#version 440 core

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 diffuse;
layout (location = 2) out vec3 normal;
layout (location = 3) out float roughness;
layout (location = 4) out float metalness;

struct Material 
{
	sampler2D diffuse_map; 
	sampler2D normal_map; 
	sampler2D metallicRoughness_map;
};

uniform Material pbr_material;

in vec3 o_position; 
in mat3 o_TBN; 
in vec2 o_uv; 

void main()
{
	position = o_position;

	vec3 nonlinear_diffse = texture(pbr_material.diffuse_map, o_uv).rgb; 
	diffuse.r = pow(nonlinear_diffse.r, 2.2f);
	diffuse.g = pow(nonlinear_diffse.g, 2.2f);
	diffuse.b = pow(nonlinear_diffse.b, 2.2f);

	vec3 sampled_normal = texture(pbr_material.normal_map, o_uv).rgb; 
	sampled_normal = sampled_normal * 2.0f - 1.0f; 
	normal = normalize(o_TBN * sampled_normal);

	metalness = texture(pbr_material.metallicRoughness_map, o_uv).b;

	roughness = texture(pbr_material.metallicRoughness_map, o_uv).g;
}