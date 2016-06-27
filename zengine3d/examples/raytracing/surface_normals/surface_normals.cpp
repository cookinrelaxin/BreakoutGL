#include <iostream>

#include <ray.h>

float hit_sphere(const glm::vec3& center, const float radius, const ray& r) {
    glm::vec3 oc = r.origin() - center;
    const float a = glm::dot(r.direction(), r.direction());
    const float b = 2.0f * glm::dot(oc, r.direction());
    const float c = glm::dot(oc, oc) - radius*radius;
    const float discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return -1.0f;
    }
    else {
       return (-b - sqrt(discriminant)) / (2.0f * a); 
    }
}

glm::vec3 color(const ray& r) {
    float t = hit_sphere(glm::vec3(0.0f,0.0f,-1.0f), 0.5f, r);
    if (t > 0.0f) {
        glm::vec3 N = glm::normalize(r.point_at_parameter(t) - glm::vec3(0.0f, 0.0f, -1.0f));
        return 0.5f * glm::vec3(N.x+1.0f, N.y+1.0f, N.z+1.0f);
    }
    glm::vec3 unit_direction = glm::normalize(r.direction());
    t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f-t) * glm::vec3(1.0f,1.0f,1.0f) + t*glm::vec3(0.5f, 0.7f, 1.0f);
}

int main(int argc, const char *argv[]) {
    const int screenWidth = 1024;
    const int screenHeight = 512;

    std::cout << "P3\n" << screenWidth << " " << screenHeight << "\n255" << std::endl;

    const glm::vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    const glm::vec3 horizontal(4.0f, 0.0f, 0.0f);
    const glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    const glm::vec3 origin(0.0f, 0.0f, 0.0f);

    for (int j = screenHeight-1; j >= 0; j--) {
        for (int i = 0; i < screenWidth; i++) {
            float u = float(i) / float(screenWidth);
            float v = float(j) / float(screenHeight);
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
