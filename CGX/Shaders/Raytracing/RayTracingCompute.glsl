#version 440 core

layout (local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout (binding = 0, rgba16f) uniform writeonly image2D output_image;

layout(std430, binding = 1) buffer Positions
{
    vec3 positions[];
};

layout(std430, binding = 2) buffer Faces
{
    unsigned int indices[];
};

const float EPSILON = 0.0001f;

// in radians
uniform float FOV; 

uniform mat4 camera_to_world; 
uniform vec3 cam_pos; 

struct Triangle 
{
    vec3 positions[3];
    vec3 normals[3];
    vec3 uvs[3];
};

struct Hit
{
    float t0;
    vec3 p0;
    vec3 barycentric_coord;
    bool is_hit;
};

struct Ray
{
    vec3 origin;
    vec3 direction;

	vec3 Point_At(float t)
    {
        return origin + t * direction;
    }

};

Triangle triangles[13];

Hit RayTriangleIntersect(Triangle triangle, Ray ray, const bool isSingleSided)
{
    Hit hit; 

    vec3 v0 = triangle.positions[1] - triangle.positions[0]; 
    vec3 v1 = triangle.positions[2] - triangle.positions[0]; 
    vec3 pvec = cross(ray.direction, v1); 
    float det = dot(pvec, v0);

    if(isSingleSided)
    {
        if(det < EPSILON)
        {
            hit.is_hit = false; 
            return hit; 
        }
    }
    else
    {
        if(abs(det) < EPSILON)
        {
            hit.is_hit = false; 
            return hit; 
        }
    }

    float invDet = 1.0f / det; 

    vec3 tvec   = ray.origin - triangle.positions[0]; 
    vec3 qvec = cross(tvec, v0);
    
    float t = dot(qvec, v1) * invDet;

    if(t < 0)
    {
        hit.is_hit = false; 
        return hit; 
    }

    float u = dot(pvec, tvec) * invDet;

    if(u < 0 || u > 1)
    {
        hit.is_hit = false; 
        return hit; 
    }


    float v = dot(qvec, ray.direction) * invDet; 

    if(v < 0 || u + v > 1)
    {
        hit.is_hit = false; 
        return hit; 
    }

    hit.is_hit = true;
    hit.t0 = t;
    hit.p0 = ray.Point_At(t);
    hit.barycentric_coord = vec3(u, v, 1.0f - u - v);
    return hit;
}

vec4 CastRay(vec2 coord)
{    
    Ray ray;
    ray.origin    = vec3(camera_to_world * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0f));
    ray.direction = vec3(camera_to_world * vec4(coord, -1.0f, 0.0f));

    float closest_hit = float(1e12);
    vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    for(int i = 0; i < 2; ++i)
    {
        Hit hit = RayTriangleIntersect(triangles[i], ray, false);

        if(hit.is_hit == false) continue; 

        if(hit.t0 < closest_hit)
        {
            vec3 red = vec3(1.0f, 0.0f, 0.0f) * hit.barycentric_coord.x;
            vec3 green = vec3(0.0f, 1.0f, 0.0f) * hit.barycentric_coord.y;
            vec3 blue = vec3(0.0f, 0.0f, 1.0f) * hit.barycentric_coord.z;

            color = vec4(red + green + blue, 1.0f);
            closest_hit = hit.t0;
        }
    }

    return color; 
}


void main()
{
	ivec2 dim = imageSize(output_image);

	uvec2 pixel_coord = gl_GlobalInvocationID.xy;

	if(pixel_coord.x >= dim.x || pixel_coord.y >= dim.y)
	{
		return; 
	}

    float aspect = float(dim.x) / float(dim.y);
    float FOV_scalar = tan(FOV * 0.5f);

    vec2 NDC = vec2((float(pixel_coord.x) + 0.5f) / float(dim.x),
        (float(pixel_coord.y) + 0.5f) / float(dim.y));

    vec2 screen_space = NDC * 2.0f - 1.0f;

    screen_space.x *= aspect;
    screen_space *= FOV_scalar;

    triangles[0].positions[0] = positions[indices[0]] / 10.0;
    triangles[0].positions[1] = positions[indices[1]] / 10.0;
    triangles[0].positions[2] = positions[indices[2]] / 10.0;

    triangles[1].positions[0] = positions[indices[3]] / 10.0;
    triangles[1].positions[1] = positions[indices[4]] / 10.0;
    triangles[1].positions[2] = positions[indices[5]] / 10.0;


    vec4 color = CastRay(screen_space);

	imageStore(output_image, ivec2(pixel_coord), color);
}