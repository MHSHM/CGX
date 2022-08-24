#version 440 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 views[6]; 

out vec3 o_frag_pos; 

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            vec4 position = gl_in[i].gl_Position;
            o_frag_pos = vec3(position);
            gl_Position = proj * views[face] * position;
            EmitVertex();
        }
        EndPrimitive();
    }
}  