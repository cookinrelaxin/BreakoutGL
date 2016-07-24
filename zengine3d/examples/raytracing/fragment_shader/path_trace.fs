//#version 330 core
#version 410 core

#define PI 3.14159
#define NUM_SPHERES 4
//#define NUM_SPHERES 3
//#define NUM_SPHERES 1

in vec2 TexCoords;
out vec4 out_color;

uniform vec2 resolution;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 albedo;
    float fuzz;
    float ref_idx;
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u,v,w;
    float lens_radius;
};

struct HitRecord {
    float t;
    vec3 position;
    vec3 normal;
    Material material;
};

bool go_white;
uniform int time;
int seed;
uniform Sphere spheres[NUM_SPHERES];

uniform Camera camera;

vec3 reflect(vec3 v, vec3 n) {
    return vec3(v - 2.0f * dot(v,n) * n);
}

float InterleavedGradientNoise(vec2 fragPos) {
    const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(fragPos, magic.xy)));
}

float frand(){
    seed = (seed * 0x5DECE66D + 0xB) & ((1 << 48) - 1);
    return abs(seed) / ((1 << 48) - 1.0f);
    // int s = (seed * 0x5DECE66D + 0xB) & ((1 << 48) - 1);
    // return abs(s) / ((1 << 48) - 1.0f);
}

vec3 randomInUnitDisk() {
    int count = 0;
    const int max_count = 100;
    vec3 p;
    do {
        count++;
        p = 2.0f * vec3(frand(), frand(), 0.0f) - vec3(1.0f, 1.0f, 0.0f);
    } while (dot(p,p) >= 1.0f && count < max_count);
    if (count >= max_count)
        go_white = true;
    return p;

    // float a = frand();
    // float b = frand();
    // if (b < a) {
    //     float temp = a;
    //     a = b;
    //     b = temp;
    // }
    // return vec3(b*cos(2.0f*PI*a/b), b*sin(2*PI*a/b), 0.0f);
}

vec3 randomInUnitSphere() {
    int count = 0;
    const int max_count = 100;
    vec3 p;
    do {
        count++;
        p = 2.0f * vec3(frand(), frand(), frand()) - vec3(1.0f, 1.0f, 1.0f);
    } while (dot(p,p) >= 1.0f && count < max_count);
    if (count >= max_count)
        go_white = true;
    return p;
}

Ray getRay(float s, float t) {
    vec3 rd = camera.lens_radius * randomInUnitDisk();
    vec3 offset = camera.u * rd.x + camera.v * rd.y;
    Ray r = Ray(camera.origin + offset,
                camera.lower_left_corner + s * camera.horizontal + t * camera.vertical - camera.origin - offset);
    return r;
}

vec3 pointAtParameter(Ray r, float t) {
    vec3 p = r.origin + t*r.direction;
    return p;
}

bool hitSphere(Ray r, Sphere s, float t_min, float t_max, inout HitRecord rec) {
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

bool refract(vec3 v, vec3 n, float ni_over_nt, inout vec3 refracted) {
    vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float discriminant = 1.0f - ni_over_nt*ni_over_nt*(1.0f - dt*dt);
    if (discriminant > 0.0f) {
       refracted = ni_over_nt*(uv - n*dt) - n * sqrt(discriminant); 
       return true;
    }
    else {
       return false; 
    }
}

float schlick(float ref_idx, float cosine) {
    float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
    r0 = r0*r0;
    return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}

bool scatterDialectric(HitRecord rec, Ray r_in, inout vec3 attenuation, inout Ray r_out) {
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction, rec.normal);
    float ni_over_nt;
    attenuation = rec.material.albedo;
    vec3 refracted;
    float reflect_prob;
    float cosine;
    float ref_idx = rec.material.ref_idx;

    if (dot(r_in.direction, rec.normal) > 0.0f) {
       outward_normal = -rec.normal; 
       ni_over_nt = ref_idx;
       cosine = ref_idx * dot(r_in.direction, rec.normal) / length(r_in.direction);
    }
    else {
       outward_normal = rec.normal; 
       ni_over_nt = 1.0f / ref_idx;
       cosine = -dot(r_in.direction, rec.normal) / length(r_in.direction);
    }
    if (refract(r_in.direction, outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(ref_idx, cosine);
    }
    else {
        r_out.origin = rec.normal;
        r_out.direction = reflected;
        reflect_prob = 1.0f;
    }
    if (frand() < reflect_prob) {
        r_out.origin = rec.position;
        r_out.direction = reflected;
        /**r_out = (struct Ray)(rec.position, reflected); */
    }
    else {
        r_out.origin = rec.position;
        r_out.direction = refracted;
    }
    return true;
}

bool scatter(HitRecord rec, Ray r_in, inout vec3 attenuation, inout Ray r_out) {
    if (rec.material.ref_idx >= 1.0f) {
        return scatterDialectric(rec, r_in, attenuation, r_out);
    }
    else {
        if (frand() <= rec.material.fuzz) {
            vec3 target = rec.position + rec.normal + randomInUnitSphere();
            r_out.origin = rec.position;
            r_out.direction = target - rec.position;
            attenuation = rec.material.albedo;
            return true;
        }
        else {
            vec3 reflected = reflect(normalize(r_in.direction), rec.normal);
            r_out.origin = rec.position;
            r_out.direction = normalize(reflected + rec.material.fuzz*randomInUnitSphere());
            attenuation = rec.material.albedo;
            return dot(r_out.direction, rec.normal) > 0.0f;
        }
    }
}

bool hitList(Sphere spheres[NUM_SPHERES], Ray r, float t_min, float t_max, inout HitRecord rec) {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < NUM_SPHERES; i++) {
        if (hitSphere(r, spheres[i], t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            if (temp_rec.t < closest_so_far) {
                closest_so_far = temp_rec.t; 
                rec = temp_rec;
            }
        }
    }
    return hit_anything;
}

vec3 color(Ray r, Sphere[NUM_SPHERES] spheres) {
    const int max_bounces = 100;
    int bounceNum = 0;
    HitRecord rec;
    vec3 col = vec3(1.0f);
    while (hitList(spheres, r, 0.001f, 9999999999999999999999.0f, rec)) {
        Ray scattered;
        vec3 attenuation;
        if (bounceNum < max_bounces && scatter(rec, r, attenuation, scattered)) {
            ++bounceNum;
            r.origin = scattered.origin;
            r.direction = scattered.direction;
            col *= attenuation;
        }
        else {
            col = vec3(0.0f);
            return col;
        }
    }
    vec3 unit_direction = normalize(r.direction);
    float t = 0.5f * (unit_direction.y + 1.0f);
    return col * ((1.0f-t) * vec3(1.0f,1.0f,1.0f) + t*vec3(0.5f, 0.7f, 1.0f));
}

void main() {

    seed = time * int(TexCoords.x * resolution.x) / int(TexCoords.y * resolution.y);
    // for (int i = 0; i < 10000; ++i)
    //     frand();
    // out_color = vec4(frand(), frand(), frand(), 1.0f);
    // return;

    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;

    const uint numSamples = 1;

    vec3 col = vec3(0.0f);

    for (uint i = 0; i < numSamples; i++) {
        float u = (x + frand()) / resolution.x;
        float v = (y + frand()) / resolution.y;
        Ray r = getRay(u, v);
        col += color(r, spheres);
    }
    col /= float(numSamples);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    //col += vec3(InterleavedGradientNoise(gl_FragCoord.xy + time) / 255.0);
    vec3 result = pow(col, vec3(1.0/2.2));
    out_color = vec4(col, 1.0f);
    // if (go_white)
    //     out_color = vec4(1.0f);
    //out_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
