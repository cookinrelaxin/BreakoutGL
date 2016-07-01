#version 330 core

#define M_PI 3.14159
const float M_PI_F = 3.14159f;
const int MAX_SPHERES = 100;
const int INT_MAX = 2147483647;
/*const float FLT_MAX(*/
/*#define FLT_MAX 1.0f / 0.0f*/

struct Camera {
        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u,v,w;
        float lens_radius;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Lambertian {
    vec3 albedo;
};

struct HitRecord {
    float t;
    vec3 position;
    vec3 normal;

    Lambertian material;
};

struct Sphere {
    vec3 center;
    float radius;
    Lambertian material;
};

uniform int time;
uniform int screenWidth;
uniform int screenHeight;

uniform Sphere spheres[MAX_SPHERES];
uniform int numSpheres;
uniform int numSamples;

out vec4 out_color;

int seed;

float frand() {
    /*return fract(sin(dot(seed.xy, vec2(12.9898,78.233))) * 43758.5453);*/
    const int a = 16807;
    const int m = 2147483647;
    seed = int((seed * a))%m;
    return float(seed) / float(INT_MAX);
}

vec3 randomInUnitDisk() {
    vec3 p;
    do {
        p = 2.0f * vec3(frand(), frand(), 0.0f) - vec3(1.0f, 1.0f, 0.0f);
    } while (dot(p,p) >= 1.0f);
    return p;
}

vec3 randomInUnitSphere() {
    vec3 p;
    do {
        p = 2.0f * vec3(frand(), frand(), frand()) - vec3(1.0f, 1.0f, 1.0f);
    } while (dot(p,p) >= 1.0f);
    return p;
}

bool scatter(in HitRecord rec, in Ray r_in, inout vec3 attenuation, inout Ray r_out);
bool scatter(in HitRecord rec, in Ray r_in, inout vec3 attenuation, inout Ray r_out) {
    vec3 target = rec.position + rec.normal + randomInUnitSphere();
    r_out = Ray(rec.position, target - rec.position);
    attenuation = rec.material.albedo;
    return true;
}

vec3 pointAtParameter(in Ray r, in float t);
bool hitSphere(in Sphere s, in Ray r, in float t_min, in float t_max, inout HitRecord rec);
vec3 color(in Ray r, in Sphere[MAX_SPHERES] spheres, in int bounceNum);
bool hitList(in Sphere[MAX_SPHERES] spheres, in Ray r, in float t_min, in float t_max, inout HitRecord rec);
bool hitSphere(in Sphere s, in Ray r, in float t_min, in float t_max, inout HitRecord rec) {
    vec3 oc = r.origin - s.center;
    float a = dot(r.direction, r.direction);
    float b = dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius * s.radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0) {
       float temp = (-b - sqrt(b*b - a*c)) / a; 
       if (temp < t_max && temp > t_min) {
           rec.t = temp;
           rec.position = pointAtParameter(r, rec.t);
           rec.normal = (rec.position - s.center) / s.radius;
           rec.material = s.material;
           return true;
       }
       temp = (-b + sqrt(b*b - a*c)) / a; 
       if (temp < t_max && temp > t_min) {
           rec.t = temp;
           rec.position = pointAtParameter(r, rec.t);
           rec.normal = (rec.position - s.center) / s.radius;
           rec.material = s.material;
           return true;
       }
    }
    return false;
}

/*//recursion is forbidden on GPUs. refactor this*/
vec3 color(in Ray r, in Sphere[MAX_SPHERES] spheres, in int bounceNum) {
    /*const int max_bounces = 50;*/
    /*HitRecord rec;*/
    /*if (hitList(spheres, r, 0.001f, FLT_MAX, rec)) {*/
        /*Ray scattered;*/
        /*vec3 attenuation;*/
        /*if (bounceNum < max_bounces && scatter(rec, r, attenuation, scattered)) {*/
            /*return color(scattered, spheres, bounceNum + 1) * attenuation;*/
        /*}*/
        /*else {*/
            /*return vec3(0.0f, 0.0f, 0.0f); */
        /*}*/
    /*}*/
    /*else {*/
        vec3 unit_direction = normalize(r.direction);
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f-t) * vec3(1.0f,1.0f,1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    /*}*/
}


bool hitList(in Sphere[MAX_SPHERES] spheres, in Ray r, in float t_min, in float t_max, inout HitRecord rec) {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < numSpheres; i++) {
        if (hitSphere(spheres[i], r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t; 
            rec = temp_rec;
        }
    }
    return hit_anything;
}

Camera initCamera(vec3 lookfrom,
                  vec3 lookat,
                  vec3 vup,
                  float vfov,
                  float aspect,
                  float aperture,
                  float focus_dist);

Camera initCamera(vec3 lookfrom,
                  vec3 lookat,
                  vec3 vup,
                  float vfov,
                  float aspect,
                  float aperture,
                  float focus_dist) {
    Camera c;
    c.lens_radius = aperture / 2.0f;

    float theta = vfov * M_PI / 180.0f; 
    float half_height = tan(theta/2.0f);
    float half_width = aspect * half_height;

    c.origin = lookfrom;
    c.w = normalize(lookfrom - lookat);
    c.u = normalize(cross(vup, c.w));
    c.v = cross(c.w, c.u);

    c.lower_left_corner = c.origin - half_width * focus_dist * c.u - half_height * focus_dist * c.v - focus_dist * c.w;
    c.horizontal = 2.0f * half_width * focus_dist * c.u;
    c.vertical = 2.0f * half_height * focus_dist * c.v;

    return c;
}

vec3 pointAtParameter(in Ray r, in float t) {
    vec3 p = r.origin + t*r.direction;
    return p;
}

Ray getRay(in Camera camera, in float s, in float t) {
    vec3 rd = camera.lens_radius * randomInUnitDisk();
    vec3 offset = camera.u * rd.x + camera.v * rd.y;
    Ray r = Ray(camera.origin + offset,
                camera.lower_left_corner + s * camera.horizontal + t * camera.vertical - camera.origin - offset);
    return r;
}

void main() {

    int x = int(gl_FragCoord.x);
    int y = int(gl_FragCoord.y);

    vec3 lookfrom = vec3(0.0f, 2.0f, 4.0f);
    vec3 lookat = vec3(0.0f, 0.0f, -1.0f);
    float dist_to_focus = length(lookfrom-lookat);
    vec3 vup = vec3(0.0f, 1.0f, 0.0f);
    float aperture = 0.0f;
    float vfov = 90.0f;
    float aspect = float(screenWidth) / float(screenHeight);

    Camera cam = initCamera(lookfrom, lookat, vup, vfov, aspect, aperture, dist_to_focus);

    const float gamma = 2.2f;
    vec3 col = vec3(0.0f);

    seed = int(gl_FragCoord.x * gl_FragCoord.y + time);

    for (int i = 0; i < 1; ++i) {
        float u = (float(x) + frand()) / float(screenWidth);
        float v = (float(y) + frand()) / float(screenHeight);
        Ray r = getRay(cam, u, v);
        col += color(r, spheres, 0);
    }
    col /= float(numSamples);

    col.x = pow(col.x, 1.0f/gamma);
    col.y = pow(col.y, 1.0f/gamma);
    col.z = pow(col.z, 1.0f/gamma);

    /*color = vec4(float(x) / float(screenWidth), float(y) / float(screenHeight), 0.0f, 1.0f);*/
    out_color = vec4(col, 1.0f);
}
