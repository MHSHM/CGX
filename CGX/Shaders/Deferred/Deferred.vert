#version 440 core

layout (location = 0) in vec3 in_position; 
layout (location = 1) in vec3 in_normal; 
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

uniform mat4 mvp;
uniform mat4 model;

out vec4 o_position;
out vec2 o_uv; 
out mat3 o_TBN; 
out vec3 o_normal;

void main()
{
	gl_Position = mvp * vec4(in_position, 1.0f);
	
	o_position = model * vec4(in_position, 1.0f); 
	
    vec3 T = normalize(vec3(model * vec4(in_tangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(in_bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(in_normal,    0.0)));
    o_TBN = mat3(T, B, N);
	
	o_normal = normalize(vec3(model * vec4(in_normal, 0.0f))); 
	o_uv = in_uv; 
}