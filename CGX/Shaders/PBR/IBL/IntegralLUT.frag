#version 440 core

const float PI = 3.14159265359f;

// Schlick-GGX
float SchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySchlickGGX(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, viewDir), 0.0f);
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float ggx2  = SchlickGGX(NdotV, roughness);
    float ggx1  = SchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
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

vec3 ImportanceSampleGGX(vec2 Xi, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H = vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);

    return H;
}

in vec2 o_uv;

out vec2 color; 

void main()
{
    float NdotV = o_uv.x;
    float roughness = o_uv.y;
    
    vec3 view;
    view.x = sqrt(1.0 - NdotV * NdotV);
    view.y = NdotV;
    view.z = 0.0;

    vec3 normal = vec3(0.0f, 1.0f, 0.0f);

    vec3 up = vec3(0.0, 0.0, 1.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 orie = mat3(tangent, normal, bitangent);

    const int samples = 1024;
    
    float scale = 0.0;
    float bias = 0.0;

    for(int i = 0; i < samples; ++i)
    {
        vec2 hammersley = hammersley2d(i, samples);
        vec3 halfway = normalize(orie * ImportanceSampleGGX(hammersley, roughness));
        vec3 light_direction = normalize((2.0 * dot(view, halfway) * halfway) - view);

        float NdotL = max(light_direction.y, 0.0);
        float NdotH = max(halfway.y, 0.0);
        float VdotH = max(dot(view, halfway), 0.0);

        if(NdotL > 0.0)
        {
            float G = GeometrySchlickGGX(normal, view, light_direction, roughness);
            float G_M = (G * VdotH) / (NdotH * NdotV);
            float F = pow(1.0 - VdotH, 5.0);

            scale += (1.0 - F) * G_M;
            bias += F * G_M;
        }
    }

    scale /= samples;
    bias /= samples;

    color.r = scale; 
    color.g = bias;
}