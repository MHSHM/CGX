#version 440 core

layout (local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout (binding = 0, rgba16f) uniform writeonly image2D output_image;

// in radians
uniform float FOV; 

uniform vec3 cam_pos;
uniform mat4 inv_view_proj;
uniform vec3 light_direction;

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

    Hit Intersect(Ray ray)
    {
        float a = 1.0f;
        float b = 2.0f * (dot(ray.origin, ray.direction) - dot(center, ray.direction));
        float c = dot(ray.origin, ray.origin) + dot(center, center) - 2.0f * dot(ray.origin, center) - (radius * radius);

        float discriminator = (b * b) - (4.0f * a * c + 0.001f);

        Hit hit;

        if (discriminator >= 0)
        {
            float s0 = ((-1.0f * b) + discriminator) / 2.0f;
            float s1 = ((-1.0f * b) - discriminator) / 2.0f;

            hit.t0 = s1;
            hit.t1 = s0;

            hit.p0 = ray.Point_At(s1);
            hit.p1 = ray.Point_At(s0);

            hit.is_hit = true; 

            return hit;
        }

        hit.is_hit = false;

        return hit;
    }
};

vec4 Cast_Ray(vec3 coord)
{
    vec4 color; 

    Ray ray;
    ray.origin    = cam_pos;
    ray.direction = normalize(coord - ray.origin);

    Sphere sphere;
    sphere.center = vec3(0.0f, 0.0f, -3.0f);
    sphere.radius = 1.0f;

    Hit hit = sphere.Intersect(ray);

    if(!hit.is_hit)
    {
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    vec3 hit_point = ray.Point_At(hit.t0);

    vec3 normal = normalize(hit_point - sphere.center);
    float light_per_unit_area = dot(normal, light_direction);

    color = vec4(1.0f, 0.0f, 0.5f, 1.0f) * light_per_unit_area; 

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
    
    vec4 world_pixel = inv_view_proj * vec4(screen_space, 1.0f, 1.0f);
    world_pixel.xyzw /= world_pixel.w;

    vec4 color = Cast_Ray(vec3(world_pixel));

	imageStore(output_image, ivec2(pixel_coord), color);
}