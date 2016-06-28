#include <iostream>

#include <ray.h>
#include <rt_camera.h>
#include <sphere.h>
#include <bvh_node.h>
#include <hitable_list.h>
#include <lambertian.h>
#include <metal.h>
#include <dialectric.h>
#include <float.h>

const int NUM_BOUNCES(50.0f);

glm::vec3 color(const ray& r, hitable* world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
        //std::cout << "world hit" << std::endl;
        ray scattered;
        glm::vec3 attenuation;
        if (depth < NUM_BOUNCES && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
           return color(scattered, world, depth+1.0f) * attenuation;
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

hitable* random_scene() {
    const int n = 500;
    hitable** list = new hitable*[n+1];
    list[0] = new sphere(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, new lambertian(glm::vec3(0.5f, 0.5f, 0.5f)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
          float choose_mat = drand48();
          glm::vec3 center(a+0.9f*drand48(), 0.2f, b+0.9f*drand48());
          if (glm::length(center-glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
              if (choose_mat < 0.8f) {
                 list[i++] = new sphere(center, 0.2f, new lambertian(glm::vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))); 
              }
              else if (choose_mat < 0.95) {
                 list[i++] = new sphere(center, 0.2f, new metal(glm::vec3(0.5f*(1.0f + drand48()), 0.5f*(1.0f + drand48()), 0.5f*(1.0f + drand48())), 0.5f * drand48())); 
              }
              else {
                  list[i++] = new sphere(center, 0.2f, new dialectric(1.5f));
              }
          }
      }
    }
    list[i++] = new sphere(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, new dialectric(1.5f));
    list[i++] = new sphere(glm::vec3(-4.0f, 4.0f, 0.0f), 4.0f, new lambertian(glm::vec3(0.4f, 0.2f, 0.1f)));
    list[i++] = new sphere(glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, new metal(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f));

    //return new hitable_list(list, i);
    return new bvh_node(list, i, 0.0f, 0.0f);
}

int main(int argc, const char *argv[]) {
    const float gamma = 2.2f;
    const int screenWidth = 1600;
    const int screenHeight = 800;
    const int numSamples = 1;

    std::cout << "P3\n" << screenWidth << " " << screenHeight << "\n255" << std::endl;

    const glm::vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    const glm::vec3 horizontal(4.0f, 0.0f, 0.0f);
    const glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    const glm::vec3 origin(0.0f, 0.0f, 0.0f);

    hitable* world = random_scene();

    glm::vec3 lookfrom(3.0f, 2.0f, 2.0f);
    glm::vec3 lookat(0.0f, 0.0f, -1.0f);
    float dist_to_focus(glm::length(lookfrom-lookat));
    float aperture = 0.0f;
    rt_camera cam(lookfrom, lookat, glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, float(screenWidth)/float(screenHeight), aperture, dist_to_focus);

    const float fInvScreenWidth(1.0f / screenWidth);
    const float fInvScreenHeight(1.0f / screenHeight);
    const float fInvNumSamples(1.0f / numSamples);

    for (int j = screenHeight-1; j >= 0; --j) {
        for (int i = 0; i < screenWidth; ++i) {
            glm::vec3 col(0.0f);
            for (int k = 0; k < numSamples; ++k) {
                float u = float(drand48() + i) * fInvScreenWidth;
                float v = float(drand48() + j) * fInvScreenHeight;
                ray r = cam.get_ray(u, v);
                //glm::vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world, 0);
            }
            col *= fInvNumSamples;

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

