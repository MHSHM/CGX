#version 330 core

const float PI = 3.14159265359f;

struct PhongMaterial 
{
	sampler2D diffuse_map;
	sampler2D specular_map;

	vec3 ka;
	vec3 kd;
	vec3 ks;
	
	float shininess;
};

struct PointLight 
{
	vec3 light_position;
	vec3 light_intensity;
};

in vec3 o_position;
in vec3 o_normal;
in vec2 o_uv;

uniform PhongMaterial material;
uniform PointLight point_light;

uniform samplerCube env;
uniform sampler2D reflection_map;

out vec4 final_color;

void main()
{
	vec3 light_direction = normalize(point_light.light_position - o_position);
	float cos_theta = max(dot(light_direction, o_normal), 0.0f);
	vec3 view_direction = normalize(-o_position);
	vec3 halfway = normalize(light_direction + view_direction);
	float cos_phi = max(dot(o_normal, halfway), 0.0f);

	vec3 light_per_unit_area = point_light.light_intensity * cos_theta; 
	vec3 amount_of_reflected_light = material.kd + (material.ks * (pow(cos_phi, material.shininess) / max(cos_theta, 0.001f)));
	vec3 c1 = light_per_unit_area * amount_of_reflected_light;
	
	float reflection_intensity = 0.8f; 
	vec3 reflection_dir = -reflect(-view_direction, o_normal);
	vec3 reflection_color = texture(env, reflection_dir).rgb;
	vec3 c2 = reflection_color * reflection_intensity;

	final_color = vec4(c1 + c2, 1.0f);
}