#version 440 core

const float PI = 3.14159265359;

uniform samplerCube cube_map;

in vec3 sample_pos;

out vec4 color;

void main()
{
	vec3 normal = normalize(sample_pos);
	vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 orie = mat3(tangent, normal, bitangent);

	vec3 irr = vec3(0.0);
	
	float delta = 0.02;
	uint samples = 0;

	for(float phi = 0.0f; phi <= 2.0 * PI; phi += delta)
	{
		for(float theta = 0.0f; theta <= PI / 2.0; theta += delta)
		{
			float sinTheta = sin(theta); 
			float sinPhi   = sin(phi); 
			float cosTheta = cos(theta);
			float cosPhi   = cos(phi); 

			vec3 sample_dir = vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);

			vec3 lightDirection = sample_dir * orie;
			vec3 radiance = texture(cube_map, lightDirection).rgb;

			irr += radiance * cosTheta * sinTheta;

			++samples; 
		}
	}

	irr = irr * PI;
	irr /= float(samples);

	color = vec4(irr, 1.0);
}