#version 440 core

struct PhongMaterial 
{
	sampler2D diffuse_map;
	sampler2D specular_map;
	sampler2D normal_map; 

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
in vec2 o_uv;
in mat4 o_model;

uniform PhongMaterial material;
uniform PointLight point_light;
uniform samplerCube depth_map;

out vec4 final_color;

vec3 GetNormal()
{
	vec3 n = texture(material.normal_map, o_uv).rgb;
	n = n * 2.0f - 1.0f;
	return n; 
}

void main()
{
	vec3 mapped_normal = GetNormal();
	mapped_normal = vec3(normalize(o_model * vec4(mapped_normal, 0.0f)));
	vec3 light_direction = normalize(point_light.light_position - o_position);
	float d = length(point_light.light_position - o_position);
	float r = 1.0f; 
	float d_squared = d * d; 
	float r_squared = r * r;
	float attenuation = 2.0f / (d_squared + r_squared + (d * pow(d_squared + r_squared, 0.5f)));
	float cos_theta = max(dot(light_direction, mapped_normal), 0.0f);
	vec3 view_direction = normalize(-o_position);
	vec3 halfway = normalize(light_direction + view_direction);
	float cos_phi = max(dot(mapped_normal, halfway), 0.0f);

	vec3 light_per_unit_area = point_light.light_intensity * attenuation * cos_theta;
	vec3 amount_of_reflected_light = material.kd + (material.ks * (pow(cos_phi, material.shininess) / max(cos_theta, 0.001f)));
	vec3 c1 = light_per_unit_area * amount_of_reflected_light;

	// Shadow Calculations
	vec3 offset[20] = vec3[]
	(
		vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
		vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
		vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
		vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
		vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);

	vec3 sample_dir = o_position - point_light.light_position;
	float current_depth = length(sample_dir);
	float bias = 0.2f;
	float shadow = 0.0f;
	int samples = 20; 
	float lobe_radius = 0.02f;

	for(int i = 0; i < samples; ++i)
	{
		float closest_dist_to_light = texture(depth_map, sample_dir + offset[i] * lobe_radius).r; 
		closest_dist_to_light *= 25.0f;
		shadow += current_depth - bias > closest_dist_to_light ? 1.0f : 0.0f; 
	}
	
	shadow /= samples; 
	vec3 color = c1 + material.ka;
	final_color = vec4(color * (1.0f - shadow), 1.0f);
}