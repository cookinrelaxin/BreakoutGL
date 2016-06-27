#ifndef RT_CAMERA_H
#define RT_CAMERA_H

#include "ray.h"

class rt_camera {
    public:
        /* Functions */
        rt_camera(glm::vec3 lookfrom,
                  glm::vec3 lookat,
                  glm::vec3 vup,
                  const float vfov,
                  const float aspect,
                  const float aperture,
                  const float focus_dist);


        ray get_ray(float s, float t);

        /* Public Data */
        glm::vec3 origin;
        glm::vec3 lower_left_corner;
        glm::vec3 horizontal;
        glm::vec3 vertical;
        glm::vec3 u,v,w;
        float lens_radius;
};

#endif /* end of include guard: RT_CAMERA_H */
