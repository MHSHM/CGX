#version 330 core

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
uniform samplerCube depth_map;
uniform float far_plane; 

out vec4 final_color;

void main()
{
	vec3 light_direction = normalize(point_light.light_position - o_position);
	float d = length(point_light.light_position - o_position);
	float r = 1.0f; 
	float d_squared = d * d; 
	float r_squared = r * r;
	float attenuation = 2.0f / (d_squared + r_squared + (d * pow(d_squared + r_squared, 0.5f)));
	float cos_theta = max(dot(light_direction, o_normal), 0.0f);
	vec3 view_direction = normalize(-o_position);
	vec3 halfway = normalize(light_direction + view_direction);
	float cos_phi = max(dot(o_normal, halfway), 0.0f);

	vec3 light_per_unit_area = point_light.light_intensity * attenuation * cos_theta;
	vec3 amount_of_reflected_light = material.kd + (material.ks * (pow(cos_phi, material.shininess) / max(cos_theta, 0.001f)));
	vec3 c1 = light_per_unit_area * amount_of_reflected_light;

	// Shadow Calculations
	vec3 frag_to_light = o_position - point_light.light_position;
	float current_depth = length(frag_to_light);
	float bias = 0.2f;
	float shadow = 0.0f;
	int samples = 0;
	float lobe_radius = 0.02f;

	for(float x = -1.0; x <= 1.0; x += 1.0f)
	{
		for(float y = -1.0; y <= 1.0; y += 1.0f)
		{
			for(float z = -1.0; z <= 1.0; z += 1.0f)
			{
				float closestDepth = texture(depth_map, frag_to_light + vec3(x, y, z) * lobe_radius).r;
				closestDepth *= far_plane;
				if(current_depth > closestDepth + bias) shadow += 1.0;
				++samples; 
			}
		}
	}

	shadow /= samples;
	vec3 color = c1 + material.ka;
	final_color = vec4(color * (1.0f - shadow), 1.0f);
}