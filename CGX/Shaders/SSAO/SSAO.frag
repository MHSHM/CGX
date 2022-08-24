#version 440 core

uniform sampler2D position_map; 
uniform sampler2D normal_map; 

uniform vec3 samples[256];
uniform mat4 projection;
uniform float radius; 

in vec2 o_uv;
out float FragColor;

void main()
{
	vec3 position = texture(position_map, o_uv).rgb; 
	vec3 normal   = texture(normal_map, o_uv).rgb;

	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = normalize(cross(up, normal));
	vec3 forward = normalize(cross(normal, right));
	mat3 orie = mat3(right, normal, forward);
	
	float occlusion = 0.0f;
	
	for(int i = 0; i < 256; ++i)
	{
		vec3 sample_pos  = orie * samples[i];
		sample_pos  = position + sample_pos * radius;

		vec4 offset = vec4(sample_pos, 1.0f);
		offset      = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz  = offset.xyz * 0.5f + 0.5f;

		float bias = 0.02f;
		float sample_depth = texture(position_map, offset.xy).z;
		occlusion        += (sample_depth >= sample_pos.z + bias ? 1.0f : 0.0f);
	}

	occlusion = 1.0f - (occlusion / 256.0f);
	FragColor = occlusion;
}