#version 440 core

struct PointLight 
{
	vec3 light_position;
	vec3 light_intensity;
};

uniform sampler2D position_map;
uniform sampler2D normal_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;
uniform PointLight pointlight;

in vec2 o_uv; 

out vec4 color; 

void main()
{
	vec3 position = texture(position_map, o_uv).rgb;
	vec3 normal = texture(normal_map, o_uv).rgb;
	vec3 diffuse_color = texture(diffuse_map, o_uv).rgb;
	vec3 specular_color = texture(specular_map, o_uv).rgb;

	vec3 light_direction = normalize(pointlight.light_position - position);
	float d = length(pointlight.light_position - position);
	float r = 1.0f; 
	float d_squared = d * d;
	float r_squared = r * r;
	float attenuation = 2.0f / (d_squared + r_squared + (d * pow(d_squared + r_squared, 0.5f)));
	float cos_theta = max(dot(light_direction, normal), 0.0f);
	vec3 view_direction = normalize(-position);
	vec3 halfway = normalize(light_direction + view_direction);
	float cos_phi = max(dot(normal, halfway), 0.0f);

	vec3 light_per_unit_area = pointlight.light_intensity * attenuation * cos_theta;
	vec3 amount_of_reflected_light = diffuse_color + (specular_color * (pow(cos_phi, 100.0f) / max(cos_theta, 0.001f)));
	vec3 c = light_per_unit_area * amount_of_reflected_light;

	float kd = 0.3f;

	color = vec4(c + (0.3f * diffuse_color), 1.0f);
}