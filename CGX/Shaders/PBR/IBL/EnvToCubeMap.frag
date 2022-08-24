#version 440 core

uniform sampler2D env_map;

in vec3 o_frag_pos; 

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

out vec4 color;

void main()
{
    vec2 uv = SampleSphericalMap(normalize(o_frag_pos));

    color = texture(env_map, uv);
}