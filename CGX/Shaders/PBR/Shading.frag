#version 440 core

const float PI = 3.14159265359f;

uniform sampler2D position_map;
uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D roughness_map;
uniform sampler2D metalness_map;
uniform sampler2D ao_map;
uniform samplerCube irradiance_map;
uniform samplerCube pre_filtered_cnv_map; 
uniform sampler2D integral_LUT; 

in vec2 o_uv; 

out vec4 final_color; 

// NDF
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

// Schlick-GGX
float SchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySchlickGGX(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, viewDir), 0.0f);
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float ggx2  = SchlickGGX(NdotV, roughness);
    float ggx1  = SchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 IndirectLightingSpecular(vec3 fragPos, float roughness,
							vec3 diffuse, float metalness, vec3 normal)
{
    vec3 viewDir = normalize(-fragPos);

    float NdotV = max(dot(viewDir, normal), 0.0f);

    const float MAX_MIP_LEVEL = 4.0;
    vec3 reflection_dir = reflect(-viewDir, normal);
    vec3 pre_filtered_color = textureLod(pre_filtered_cnv_map, reflection_dir, roughness * MAX_MIP_LEVEL).rgb; 

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, diffuse, metalness);

    vec2 correction =  texture(integral_LUT, vec2(NdotV, roughness)).rg;
    float scaler    =  correction.r;
    float bias      =  correction.g;
    vec3 fresnel    =  F0 * scaler + bias;
                
    return pre_filtered_color * fresnel;

}

vec3 IndirectLightingDiffuse(vec3 fragPos, float roughness,
							vec3 diffuse, float metalness, vec3 normal)
{
    vec3 viewDir = normalize(-fragPos);

    float NdotV = max(dot(viewDir, normal), 0.0f); 

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, diffuse, metalness);

    vec3 F = fresnelSchlick(NdotV, F0, roughness);

    vec3 diff   = diffuse * texture(irradiance_map, normal).rgb * (1.0f - metalness);
    diff *= vec3(1.0f) - F;

    return diff;
}

void main()
{
	vec3 fragPos = texture(position_map, o_uv).rgb;
	vec3 normal = normalize(texture(normal_map, o_uv).rgb);
	vec3 diffuse = texture(diffuse_map, o_uv).rgb;
	float roughness = texture(roughness_map, o_uv).r;
	float metalness = texture(metalness_map, o_uv).r;
	float ao = texture(ao_map, o_uv).r;

    // indirect lighting
    vec3 indirect_diffuse   = IndirectLightingDiffuse(fragPos, roughness, diffuse, metalness, normal);
    vec3 indirect_sprecular = IndirectLightingSpecular(fragPos, roughness, diffuse, metalness, normal);

    vec3 indirect_lighting = (indirect_sprecular + indirect_diffuse) * ao;

	final_color = vec4(indirect_lighting, 1.0f);
}