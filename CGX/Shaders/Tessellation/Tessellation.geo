#version 440 core
layout (triangles) in;
layout (line_strip, max_vertices=4) out;

void main()
{
	const float Z_OFFSET = -0.1f;

	gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0f, Z_OFFSET, 0.0f);
	EmitVertex();

	gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.0f, Z_OFFSET, 0.0f);
	EmitVertex();

	gl_Position = gl_in[2].gl_Position + vec4(0.0f, 0.0f, Z_OFFSET, 0.0f);
	EmitVertex();
	
	gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0f, Z_OFFSET, 0.0f);
	EmitVertex();
}
