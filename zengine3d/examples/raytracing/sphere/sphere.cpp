#include <iostream>

#include <ray.h>

bool hit_sphere(const glm::vec3& center, const float radius, const ray& r) {
    glm::vec3 oc = r.origin() - center;
    const float a = glm::dot(r.direction(), r.direction());
    const float b = 2.0f * glm::dot(oc, r.direction());
    const float c = glm::dot(oc, oc) - radius*radius;
    const float discriminant = b*b - 4*a*c;
    return discriminant > 0;
}

glm::vec3 color(const ray& r) {
    if (hit_sphere(glm::vec3(0.0f,0.0f,-1.0f), 0.5f, r))
        return glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 unit_direction = glm::normalize(r.direction());
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f-t) * glm::vec3(1.0f,1.0f,1.0f) + t*glm::vec3(0.5f, 0.7f, 1.0f);
}

int main(int argc, const char *argv[]) {
    const int nx = 200;
    const int ny = 100;

    std::cout << "P3\n" << nx << " " << ny << "\n255" << std::endl;

    const glm::vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    const glm::vec3 horizontal(4.0f, 0.0f, 0.0f);
    const glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    const glm::vec3 origin(0.0f, 0.0f, 0.0f);

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            glm::vec3 col = color(r);

            int ir = int(255.99*col.x);
            int ig = int(255.99*col.y);
            int ib = int(255.99*col.z);

            std::cout << ir << " " << ig << " " << ib << std::endl;
        }
        
    }

    return 0;
}