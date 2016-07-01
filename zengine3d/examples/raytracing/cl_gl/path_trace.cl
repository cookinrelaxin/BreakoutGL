struct Ray;
struct Camera;
struct Pixel;
struct Sphere;
struct HitRecord;
struct HitableList;
struct Material;

bool hitList(struct HitableList* spheres, const struct Ray* r, float t_min, float t_max, struct HitRecord* rec);
bool scatter(const struct HitRecord* rec, const struct Ray* r_in, float3* attenuation, struct Ray* r_out, int* seed);
float3 reflect(const float3 v, const float3 n);
/*bool lambertian::scatter(const ray& r_in,*/
                         /*const hit_record& rec,*/
                         /*glm::vec3& attenuation,*/
                         /*ray& scattered) const {*/

float frand(int* seed);
float3 randomInUnitDisk(int* seed);
float3 randomInUnitSphere(int* seed);
float3 color(struct Ray* r, struct HitableList* spheres, int* seed, unsigned int bounceNum);
bool hitSphere(struct Sphere s, const struct Ray* r, float t_min, float t_max, struct HitRecord* rec);

struct Ray initRay(const float3 a, const float3 b);
struct Sphere initSphere(float3 center, float radius);
/*struct Camera initCamera(float3 lookfrom,*/
                         /*float3 lookat,*/
                         /*float3 vup,*/
                         /*float vfov,*/
                         /*float aspect,*/
                         /*float aperture,*/
                         /*float focus_dist,*/
                         /*int* seed);*/

struct Ray getRay(struct Camera* camera, float s, float t, int* seed);
float3 pointAtParameter(const struct Ray* r, float t);

float3 reflect(const float3 v, const float3 n) {
    return (float3)(v - 2.0f * dot(v,n) * n);
}

struct Pixel {
    float r;
    float g;
    float b;
};

struct Material {
    float3 albedo;
    float fuzz;
};

/*struct Metal {*/
    /*float3 albedo;*/
/*};*/

struct Sphere {
    float3 center;
    float radius;
    struct Material material;
};

struct Camera {
        float3 origin;
        float3 lower_left_corner;
        float3 horizontal;
        float3 vertical;
        float3 u,v,w;
        float lens_radius;

        /*int* seed;*/
};

/*struct Camera initCamera(float3 lookfrom,*/
                         /*float3 lookat,*/
                         /*float3 vup,*/
                         /*float vfov,*/
                         /*float aspect,*/
                         /*float aperture,*/
                         /*float focus_dist,*/
                         /*int* seed) {*/
    /*struct Camera c;*/
    /*c.lens_radius = aperture / 2.0f;*/

    /*const float theta = vfov * M_PI_F/180.0f; */
    /*const float half_height = tan(theta/2.0f);*/
    /*const float half_width = aspect * half_height;*/

    /*c.origin = lookfrom;*/
    /*c.w = normalize(lookfrom - lookat);*/
    /*c.u = normalize(cross(vup, c.w));*/
    /*c.v = cross(c.w, c.u);*/

    /*c.lower_left_corner = c.origin - half_width * focus_dist * c.u - half_height * focus_dist * c.v - focus_dist * c.w;*/
    /*c.horizontal = 2.0f * half_width * focus_dist * c.u;*/
    /*c.vertical = 2.0f * half_height * focus_dist * c.v;*/

    /*c.seed = seed;*/

    /*return c;*/
/*}*/

// JAVA version
float frand(int* seed) {
    *seed = (*seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    return (float)abs(*seed) / (float)INT_MAX;
}

float3 randomInUnitDisk(int* seed) {
    float3 p;
    do {
        p = 2.0f * (float3)(frand(seed), frand(seed), 0.0f) - (float3)(1.0f, 1.0f, 0.0f);
    } while (dot(p,p) >= 1.0f);
    return p;
}

float3 randomInUnitSphere(int* seed) {
    float3 p;
    do {
        p = 2.0f * (float3)(frand(seed), frand(seed), frand(seed)) - (float3)(1.0f, 1.0f, 1.0f);
    } while (dot(p,p) >= 1.0f);
    return p;
}

struct Ray {
    float3 origin;
    float3 direction;
};

struct Ray initRay(const float3 a, const float3 b) {
    struct Ray r = {a, b};
    return r;
}

struct Ray getRay(struct Camera* camera, float s, float t, int* seed) {
    float3 rd = camera->lens_radius * randomInUnitDisk(seed);
    float3 offset = camera->u * rd.x + camera->v * rd.y;
    struct Ray r = {camera->origin + offset,
                    camera->lower_left_corner + s * camera->horizontal + t * camera->vertical - camera->origin - offset};
    return r;
}

float3 pointAtParameter(const struct Ray* r, float t) {
    float3 p = r->origin + t*r->direction;
    return p;
}

struct HitRecord {
    float t;
    float3 position;
    float3 normal;

    struct Material material;
};

struct HitableList {
    __constant struct Sphere* list;
    int size;
};

bool hitList(struct HitableList* spheres, const struct Ray* r, float t_min, float t_max, struct HitRecord* rec) {
    struct HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (int i = 0; i < spheres->size; i++) {
        if (hitSphere(spheres->list[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t; 
            *rec = temp_rec;
        }
    }
    return hit_anything;
}

bool hitSphere(struct Sphere s, const struct Ray* r, float t_min, float t_max, struct HitRecord* rec) {
    float3 oc = r->origin - s.center;
    float a = dot(r->direction, r->direction);
    float b = dot(oc, r->direction);
    float c = dot(oc, oc) - s.radius * s.radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0) {
       float temp = (-b - sqrt(b*b - a*c)) / a; 
       if (temp < t_max && temp > t_min) {
           rec->t = temp;
           rec->position = pointAtParameter(r, rec->t);
           rec->normal = (rec->position - s.center) / s.radius;
           rec->material = s.material;
           return true;
       }
       temp = (-b + sqrt(b*b - a*c)) / a; 
       if (temp < t_max && temp > t_min) {
           rec->t = temp;
           rec->position = pointAtParameter(r, rec->t);
           rec->normal = (rec->position - s.center) / s.radius;
           rec->material = s.material;
           return true;
       }
    }
    return false;
}

/*float3 reflect(const float3 v, const float3 n) {*/
    /*return (float3)(v - 2.0f * dot(v,n) * n);*/
/*}*/

bool scatter(const struct HitRecord* rec, const struct Ray* r_in, float3* attenuation, struct Ray* r_out, int* seed) {
    if (frand(seed) <= rec->material.fuzz) {
        float3 target = rec->position + rec->normal + randomInUnitSphere(seed);
        r_out->origin = rec->position;
        r_out->direction = target - rec->position;
        *attenuation = rec->material.albedo;
        return true;
    }
    else {
        float3 reflected = reflect(normalize(r_in->direction), rec->normal);
        r_out->origin = rec->position;
        r_out->direction = normalize(reflected + rec->material.fuzz*randomInUnitSphere(seed));
        *attenuation = rec->material.albedo;
        return dot(r_out->direction, rec->normal) > 0.0f;
    }
    return false;
}

/*glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);*/
/*scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());*/
/*attenuation = albedo;*/
/*return glm::dot(scattered.direction(), rec.normal) > 0.0f;*/

//recursion is forbidden on GPUs. refactor this
/*float3 color(struct Ray* r, struct HitableList* spheres, int* seed, unsigned int bounceNum) {*/
    /*const unsigned int max_bounces = 50;*/
    /*struct HitRecord rec;*/
    /*if (hitList(spheres, r, 0.001f, FLT_MAX, &rec)) {*/
        /*struct Ray scattered;*/
        /*float3 attenuation;*/
        /*if (bounceNum < max_bounces && scatter(&rec, r, &attenuation, &scattered, seed)) {*/
            /*return color(&scattered, spheres, seed, bounceNum + 1) * attenuation;*/
        /*}*/
        /*else {*/
            /*return (float3)(0.0f, 0.0f, 0.0f); */
        /*}*/
    /*}*/
    /*else {*/
        /*float3 unit_direction = normalize(r->direction);*/
        /*float t = 0.5f * (unit_direction.y + 1.0f);*/
        /*return (1.0f-t) * (float3)(1.0f,1.0f,1.0f) + t*(float3)(0.5f, 0.7f, 1.0f);*/
    /*}*/
/*}*/

float3 color(struct Ray* r, struct HitableList* spheres, int* seed, unsigned int bounceNum) {
    const unsigned int max_bounces = 100;
    struct HitRecord rec;
    float3 col = (float3)(1.0f, 1.0f, 1.0f);
    while (hitList(spheres, r, 0.001f, FLT_MAX, &rec)) {
        struct Ray scattered;
        float3 attenuation;
        if (bounceNum < max_bounces && scatter(&rec, r, &attenuation, &scattered, seed)) {
            ++bounceNum;
            r->origin = scattered.origin;
            r->direction = scattered.direction;
            col *= attenuation;
        }
        else {
            col = (float3)(0.0f, 0.0f, 0.0f); 
            return col;
        }
    }
    float3 unit_direction = normalize(r->direction);
    float t = 0.5f * (unit_direction.y + 1.0f);
    return col * ((1.0f-t) * (float3)(1.0f,1.0f,1.0f) + t*(float3)(0.5f, 0.7f, 1.0f));
}

__kernel void color_pixel(__global struct Pixel* pixels,
                          __global int* seeds,
                          int s,
                          __constant struct Sphere* spheres,
                          const unsigned int numSpheres,
                          const unsigned int numPixels,
                          const unsigned int maxSamples,
                          const unsigned int screenWidth,
                          const unsigned int screenHeight,
                          struct Camera cam) {
    // range from 0..numPixels
    unsigned int n = get_global_id(0);

    int seed = s*seeds[n];

    int x = n % screenWidth;
    int y = screenHeight - n / screenWidth;

    struct HitableList sphereList;
    sphereList.list = spheres;
    sphereList.size = numSpheres;

    const float gamma = 2.2f;
    float3 col = (float3)(0.0f);

    const float fInvScreenWidth = 1.0f / screenWidth;
    const float fInvScreenHeight = 1.0f / screenHeight;

    float diff = length((float2)(screenWidth/2, screenHeight/2) - (float2)(x, y));
    unsigned int numSamples = maxSamples / (int)pow(diff, 1.1f);
    float aperture = 0.005f;
    cam.lens_radius = aperture / 2.0f;

    for (unsigned int i = 0; i < numSamples; ++i) {
        float u = ((float)x + frand(&seed)) * fInvScreenWidth;
        float v = ((float)y + frand(&seed)) * fInvScreenHeight;
        struct Ray r = getRay(&cam, u, v, &seed);
        col += color(&r, &sphereList, &seed, 0);
    }
    col /= (float)numSamples;

    col.x = pow(col.x, 1.0f/gamma);
    col.y = pow(col.y, 1.0f/gamma);
    col.z = pow(col.z, 1.0f/gamma);

    if (n < numPixels) {
        pixels[n].r = col.x;
        pixels[n].g = col.y;
        pixels[n].b = col.z;
    }
}
