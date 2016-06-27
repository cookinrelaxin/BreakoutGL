#ifndef RT_CAMERA_H
#define RT_CAMERA_H

#include "ray.h"

class rt_camera {
    public:
        /* Functions */
        rt_camera(glm::vec3 lower_left_corner = glm::vec3(-2.0f, -1.0f, -1.0f),
                  glm::vec3 horizontal        = glm::vec3(4.0f, 0.0f, 0.0f),
                  glm::vec3 vertical          = glm::vec3(0.0f, 2.0f, 0.0f),
                  glm::vec3 origin            = glm::vec3(0.0f, 0.0f, 0.0f))
            : lower_left_corner(lower_left_corner)
            , horizontal(horizontal)
            , vertical(vertical)
            , origin(origin) {}

        ray get_ray(float u, float v);

        /* Public Data */
        glm::vec3 origin;
        glm::vec3 lower_left_corner;
        glm::vec3 horizontal;
        glm::vec3 vertical;
};

#endif /* end of include guard: RT_CAMERA_H */
