#version 440 core

layout (local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout (binding = 0, rgba16f) uniform writeonly image2D output_image;

const float EPSILON = 0.00001f;

// in radians
uniform float FOV;

uniform mat4 camera_to_world; 

struct Hit
{
    float t0;
    float t1;
    vec3 p0;
    vec3 p1;
    vec3 normal;
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

struct Sphere
{
    vec3 center;
    float radius;
};

Hit RaySphereIntersect(Sphere sphere, Ray ray, const bool backface_cull)
{
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - pow(sphere.radius, 2.0f);
    float discriminant = b*b - 4.0f*a*c;

    Hit hit;
    hit.is_hit = false;

    if (discriminant >= 0.0f)
    {
        float t0 = ((-1.0f * b) + discriminant) / 2.0f;
        float t1 = ((-1.0f * b) - discriminant) / 2.0f;

        hit.t0 = t0;
        hit.t1 = t1;

        hit.p0 = ray.Point_At(t0);
        hit.p1 = ray.Point_At(t1);
        hit.normal = normalize(hit.p1 - sphere.center);

        hit.is_hit = true;

        return hit;
    }

    return hit;
}

vec4 CastRay(vec2 coord)
{
    Ray ray;
    ray.origin    = vec3(camera_to_world * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0f));
    ray.direction = vec3(camera_to_world * vec4(coord, -1.0f, 0.0f));

    Sphere spheres[3];

    for(int i = 0; i < 3; ++i)
    {
        spheres[i].center = vec3((i + 1) * 3, 0.0f, 0.0f);
        spheres[i].radius = 1.0f;
    }

    vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float closest_hit = 1e11;
    
    for(int i = 0; i < 3; ++i)
    {
        Hit hit = RaySphereIntersect(spheres[i], ray, false);

        if(hit.is_hit && hit.t1 < closest_hit)
        {
            closest_hit = hit.t1;
            color = vec4(0.8f, 0.3f, 1.0f, 1.0f);
        }
    }

    return color;
}


void main()
{
	ivec2 dim = imageSize(output_image);

	uvec2 pixel_coord = gl_GlobalInvocationID.xy;

    float aspect = float(dim.x) / float(dim.y);
    float FOV_scalar = tan(FOV * 0.5f);

    vec2 NDC = vec2((float(pixel_coord.x) + 0.5f) / float(dim.x),
        (float(pixel_coord.y) + 0.5f) / float(dim.y));

    vec2 screen_space = NDC * 2.0f - 1.0f;

    screen_space.x *= aspect;
    screen_space *= FOV_scalar;

    vec4 color = CastRay(screen_space);

	imageStore(output_image, ivec2(pixel_coord), color);
}