#version 440 core

in vec2 o_uv; 

uniform sampler2D ssao;

out float FragColor;

void main()
{
	vec2 dim = textureSize(ssao, 0);
	vec2 pixel_coord = vec2(o_uv.x * dim.x + 0.5f, o_uv.y * dim.y + 0.5f);
	float avg_occlusion = 0.0f;

	for(int i = -2; i < 2; ++i)
	{
		for(int j = -2; j < 2; ++j)
		{
			vec2 neighbour = pixel_coord + vec2(i, j);
			vec2 normalized = neighbour / dim;
			avg_occlusion += texture(ssao, normalized).r;
		}
	}

	FragColor = avg_occlusion / 16.0f;
}