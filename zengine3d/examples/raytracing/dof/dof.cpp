#include <iostream>

#include <ray.h>
#include <rt_camera.h>
#include <sphere.h>
#include <lambertian.h>
#include <metal.h>
#include <dialectric.h>
#include <hitable_list.h>
#include <float.h>

glm::vec3 color(const ray& r, hitable* world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
        ray scattered;
        glm::vec3 attenuation;
        if (depth < 50.0f && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
           return attenuation*color(scattered, world, depth+1.0f);
        }
        else {
           return glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }
    else {
        glm::vec3 unit_direction = glm::normalize(r.direction());
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f-t) * glm::vec3(1.0f,1.0f,1.0f) + t*glm::vec3(0.5f, 0.7f, 1.0f);
    }
}

int main(int argc, const char *argv[]) {
    const float gamma = 2.2f;
    const int screenWidth = 256;
    const int screenHeight = 128;
    const int numSamples = 100;

    std::cout << "P3\n" << screenWidth << " " << screenHeight << "\n255" << std::endl;

    const glm::vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    const glm::vec3 horizontal(4.0f, 0.0f, 0.0f);
    const glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    const glm::vec3 origin(0.0f, 0.0f, 0.0f);

    hitable *list[5];
    list[0] = new sphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, new lambertian(glm::vec3(0.1f, 0.2f, 0.5f)));
    list[1] = new sphere(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, new lambertian(glm::vec3(0.8f, 0.8f, 0.0f)));
    list[2] = new sphere(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, new metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.0f));
    list[3] = new sphere(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, new dialectric(1.5f));
    list[4] = new sphere(glm::vec3(-1.0f, 0.0f, -1.0f), -0.45f, new dialectric(1.5f));
    hitable* world = new hitable_list(list, 5);

    glm::vec3 lookfrom(3.0f, 3.0f, 2.0f);
    glm::vec3 lookat(0.0f, 0.0f, -1.0f);
    float dist_to_focus(glm::length(lookfrom-lookat));
    float aperture = 2.0f;
    rt_camera cam(lookfrom, lookat, glm::vec3(0.0f, 1.0f, 0.0f), 20.0f, float(screenWidth)/float(screenHeight), aperture, dist_to_focus);

    for (int j = screenHeight-1; j >= 0; j--) {
        for (int i = 0; i < screenWidth; i++) {
            glm::vec3 col(0.0f);
            for (int k = 0; k < numSamples; k++) {
                float u = float(drand48() + i) / float(screenWidth);
                float v = float(drand48() + j) / float(screenHeight);
                ray r = cam.get_ray(u, v);
                //glm::vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world, 0);
            }
            col /= float(numSamples);

            col.x = powf(col.x, 1.0f/gamma);
            col.y = powf(col.y, 1.0f/gamma);
            col.z = powf(col.z, 1.0f/gamma);

            int ir = int(255.99*col.x);
            int ig = int(255.99*col.y);
            int ib = int(255.99*col.z);

            std::cout << ir << " " << ig << " " << ib << std::endl;
        }

    }

    return 0;
}
