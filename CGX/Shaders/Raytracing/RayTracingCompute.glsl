#version 440 core

layout (local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout (binding = 0, rgba16f) uniform writeonly image2D output_image;

const float EPSILON = 0.00001f;
const float PI = 3.14159265359f;
const int MAX_SPHERES = 10;
// in radians
uniform float FOV;

uniform mat4 camera_to_world;

struct PointLight
{
    vec3 position;
    vec3 color;
};

struct Sphere
{
    vec3 center;
    float radius;
};

uniform PointLight pointlight;
uniform Sphere spheres[MAX_SPHERES];
uniform vec3 spheres_color[MAX_SPHERES];
uniform int spheres_count;

struct Hit
{
    float t0;
    float t1;
    vec3 p0;
    vec3 p1;
    vec3 normal;
    bool is_hit;
    vec4 color;
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

Hit RaySphereIntersect(Sphere sphere, Ray ray, const bool backface_cull)
{
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - pow(sphere.radius, 2.0f);
    float discriminant = b * b - 4.0f * a * c;

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

vec4 CastRay(Ray ray, float bounces = 1.0f)
{
    Hit final_hit;
    final_hit.t1 = 1e11;
    final_hit.is_hit = false;
    final_hit.color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if(bounces <= 0.0)
        return final_hit.color;

    // see if the ray hits anything in the world
    for(int i = 0; i < spheres_count; ++i)
    {
        Hit hit = RaySphereIntersect(spheres[i], ray, false);
        if(hit.is_hit && hit.t1 < final_hit.t1)
        {
            final_hit.is_hit = hit.is_hit;
            final_hit.t1 = hit.t1;
            final_hit.p1 = hit.p1;
            final_hit.normal = hit.normal;
            final_hit.color = vec4(spheres_color[i], 1.0f);
        }
    }

    if(final_hit.is_hit)
    {
        return final_hit.color;
    }

    float t = 0.5f * (ray.direction.y + 1.0f);
    vec3 miss_color = (1.0f - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    vec4 color = vec4(miss_color, 1.0f);
    return color;
}

float random(vec3 co) {
    return fract(sin(dot(co.xyz, vec3(12.9898, 78.233, 151.7182))) * 43758.5453);
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

    vec3 origin    = vec3(camera_to_world * vec4(0.0f, 0.0f, 0.0f, 1.0f));
    vec3 direction = vec3(camera_to_world * (vec4(screen_space, -1.0f, 0.0f) - vec4(origin.xy, 0.0f, 0.0f)));

    // antialiasing
    vec4 color = vec4(0.0f);
    int rays_count = 0;
    vec2 texel_size = 1.0f / dim;
    for(int i = 0; i < 8; ++i)
    {
        vec2 offset = vec2(random(vec3(pixel_coord.x, i, 1.0f)), random(vec3(pixel_coord.y, i, 1.0f)));

        Ray ray;
        ray.origin = origin;
        ray.direction = direction + vec3(offset, 0.0f) * vec3(texel_size, 1.0f);

        color += CastRay(ray);

        ++rays_count;
    }

	imageStore(output_image, ivec2(pixel_coord), color / float(rays_count));
}