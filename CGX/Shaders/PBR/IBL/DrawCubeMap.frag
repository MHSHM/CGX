#version 440 core

uniform samplerCube cube_map;
uniform sampler2D depth; 
uniform vec2 window_dim; 

in vec3 sample_pos; 

out vec4 color; 

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0f, 1.0f);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / window_dim;
	float g_depth = texture(depth, uv).r; 
	
	if(g_depth < 1.0)
	{
		discard;
	}

	float gamma = 2.2f;

	vec3 hdr_color = texture(cube_map, sample_pos).rgb;

	vec3 toned = ACESFilm(hdr_color);

	color = vec4(pow(toned, vec3(1.0f / gamma)), 1.0f);
}