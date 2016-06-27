#include "rt_camera.h"

glm::vec3 random_in_unit_disk() {
    glm::vec3 p;
    do {
        p = 2.0f * glm::vec3(drand48(), drand48(), 0.0f) - glm::vec3(1.0f, 1.0f, 0.0f);
    } while (glm::dot(p,p) >= 1.0f);
    return p;
}

rt_camera::rt_camera(glm::vec3 lookfrom,
                     glm::vec3 lookat,
                     glm::vec3 vup,
                     const float vfov,
                     const float aspect,
                     const float aperture,
                     const float focus_dist) {
    lens_radius = aperture / 2.0f;

    const float theta = vfov * (float)M_PI/180.0f; 
    const float half_height = tanf(theta/2.0f);
    const float half_width = aspect * half_height;

    origin = lookfrom;
    w = glm::normalize(lookfrom - lookat);
    u = glm::normalize(glm::cross(vup, w));
    v = cross(w, u);

    lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w;
    horizontal = 2*half_width*focus_dist*u;
    vertical = 2*half_height*focus_dist*v;
}

ray rt_camera::get_ray(float s, float t) {
    glm::vec3 rd = lens_radius*random_in_unit_disk();
    glm::vec3 offset = u * rd.x + v * rd.y;
    return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
}
