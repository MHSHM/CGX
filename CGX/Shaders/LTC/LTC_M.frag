#version 440 core

const float PI = 3.14159265359f;
const float LUT_SIZE  = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;
const int MAX_LIGHT_SOURCES = 10;

struct QuadLight 
{
	vec3 vertices[4]; 
	vec3 intensity; 
};

uniform sampler2D position_map;
uniform sampler2D normal_map;
uniform sampler2D diffuse_map;
uniform sampler2D roughness_map;
uniform sampler2D metalness_map;
uniform sampler2D ao_map;
uniform sampler2D ltc_1;
uniform sampler2D ltc_2;

uniform QuadLight quadlight[MAX_LIGHT_SOURCES];
uniform bool flag;
uniform int active_quad_lights;

in vec2 o_uv; 

out vec4 final_color; 

vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

mat3 mul(mat3 m1, mat3 m2)
{
    return m1 * m2;
}

vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4])
{
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N*dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = mul(Minv, transpose(mat3(T1, T2, N)));

	vec3 L[5];
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);
    L[3] = Minv * (points[3] - P);

    vec3 dir = normalize(points[0].xyz - P);
    vec3 lightNormal = normalize(cross(points[1] - points[0], points[3] - points[0]));
    bool behind = (dot(dir, lightNormal) < 0.0);

    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    float sum = 0.0;
    vec3 vsum = vec3(0.0);

    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    float len = length(vsum);
    float z = vsum.z/len;
            
    vec2 uv = vec2(z*0.5 + 0.5, len);
    uv = uv * LUT_SCALE + LUT_BIAS;

    float scale = texture(ltc_2, uv).w;

    sum = len * scale;

    if (behind)
    {
        sum = 0.0;
    }

    vec3 Lo_i = vec3(sum, sum, sum);

    return Lo_i;
}

float saturate(float v)
{
    return clamp(v, 0.0, 1.0);
}

void main()
{
	vec3 fragPos = texture(position_map, o_uv).rgb;
	vec3 normal = normalize(texture(normal_map, o_uv).rgb);
	vec3 diffuse = texture(diffuse_map, o_uv).rgb;
	float roughness = texture(roughness_map, o_uv).r; 
	float metalness = texture(metalness_map, o_uv).r;
	float ao = texture(ao_map, o_uv).r;

    vec3 F0 = vec3(0.04f);
	F0 = mix(F0, diffuse, metalness);

    vec3 viewDir = normalize(-fragPos); 

    float NdotV = max(dot(normal, viewDir), 0.0f);
    
    vec2 uv = vec2(roughness, sqrt(1.0f - NdotV));
    uv = uv * LUT_SCALE + LUT_BIAS;

    vec4 t1 = texture(ltc_1, uv);
    vec4 t2 = texture(ltc_2, uv);

    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    vec3 col = vec3(0.0f);

    for(int i = 0; i < active_quad_lights; ++i)
    {
        vec3 spec = LTC_Evaluate(normal, viewDir, fragPos, Minv, quadlight[i].vertices);

        spec *= (F0 * t2.x) + ((vec3(1.0f) - F0) * t2.y);

        vec3 diff = LTC_Evaluate(normal, viewDir, fragPos, mat3(1.0), quadlight[i].vertices);
        diff *= (1.0f - metalness);
    
        col += quadlight[i].intensity * (spec + (diffuse * diff));
    }

    float ambient_intensity = 0.03f;
	vec3 ambient = diffuse * ambient_intensity * ao;

    final_color = vec4(col + ambient, 1.0f);
}