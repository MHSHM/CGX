#version 430 core

uniform sampler2D scene_refered_image;
uniform float scene_exposure; 

in vec2 o_uv; 

out vec4 FragColor; 

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
	float gamma = 2.2f;

	vec3 hdr_color = texture(scene_refered_image, o_uv).rgb;
	
	vec3 toned = ACESFilm(hdr_color);

	FragColor = vec4(pow(toned, vec3(1.0f / gamma)), 1.0f);
}
