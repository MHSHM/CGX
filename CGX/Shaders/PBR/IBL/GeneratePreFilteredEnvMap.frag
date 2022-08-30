#version 440 core

const float PI = 3.14159265359f;
const int N = 1024; 

uniform samplerCube cube_map;
uniform float roughness;

in vec3 sample_pos;

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H = vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);	

    return H;
}

float GGX_Distribution(vec3 normal, vec3 halfway, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, halfway), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley2d(uint i, uint N)
{
	return vec2(float(i) / float(N), radicalInverse_VdC(i));
}

out vec4 color;

void main()
{
	vec3 normal = normalize(sample_pos);
	
	vec3 view = normal;
	vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 orie = mat3(tangent, normal, bitangent);

	vec3 prefiltered_color = vec3(0.0);

	for(int i = 0; i < N; ++i)
	{
		vec2 h = hammersley2d(i, N);

		vec3 halfway = normalize(orie * ImportanceSampleGGX(h, normal, roughness));
		vec3 light_direction = normalize(2.0 * dot(view, halfway) * halfway - view);

		float LdotN = max(dot(light_direction, normal), 0.0f); 

		/*
		if(LdotN > 0.0)
		{
			prefiltered_color += textureLod(cube_map, light_direction, 0.0).rgb;
		}
		*/
		
		if(LdotN > 0.0)
		{
			float NdotH = max(dot(normal, halfway), 0.0f); 
			float HdotV = max(dot(halfway, view), 0.0f);

			float D   = GGX_Distribution(normal, halfway, roughness);
			float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

			float resolution = 512.0;
			float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(N) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			prefiltered_color += textureLod(cube_map, light_direction, mipLevel).rgb;
		}
		
	}

	color = vec4(prefiltered_color / N, 1.0);
}