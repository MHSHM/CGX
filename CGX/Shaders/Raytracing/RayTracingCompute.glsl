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

Hit RaySphereIntersect(Sphere sphere, Ray ray, const bool isSingleSided)
{
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b*b - 4*a*c;

    Hit hit;

    if (discriminant >= 0)
    {
        float s0 = ((-1.0f * b) + discriminant) / 2.0f;
        float s1 = ((-1.0f * b) - discriminant) / 2.0f;

        if(s0 < 0.0)
        {
            hit.is_hit = false;
            return hit;
        }

        hit.t0 = s0;
        hit.t1 = s1;

        hit.p0 = ray.Point_At(s0);
        hit.p1 = ray.Point_At(s1);

        hit.is_hit = true; 

        return hit;
    }

    hit.is_hit = false;

    return hit;
}

vec4 CastRay(vec2 coord)
{
    Ray ray;
    ray.origin    = vec3(camera_to_world * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0f));
    ray.direction = vec3(camera_to_world * vec4(coord, -1.0f, 0.0f));

    Sphere sphere; 
    sphere.center = vec3(0.0f, 0.0f, 0.0f);
    sphere.radius = 1.0f;

    Hit hit = RaySphereIntersect(sphere, ray, false);

    if(!hit.is_hit)
    {
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    return vec4(1.0f, 0.0f, 0.0f, 1.0f);
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

    vec4 color = CastRay(screen_space);

	imageStore(output_image, ivec2(pixel_coord), color);
}