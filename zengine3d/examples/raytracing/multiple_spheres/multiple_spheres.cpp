#include <iostream>

#include <ray.h>
#include <sphere.h>
#include <hitable_list.h>
#include <float.h>

glm::vec3 color(const ray& r, hitable* world) {
    hit_record rec;
    if (world->hit(r, 0.0f, MAXFLOAT, rec)) {
       return 0.5f * glm::vec3(rec.normal.x + 1, rec.normal.y+1, rec.normal.z+1); 
    }
    else {
        glm::vec3 unit_direction = glm::normalize(r.direction());
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f-t) * glm::vec3(1.0f,1.0f,1.0f) + t*glm::vec3(0.5f, 0.7f, 1.0f);
    }
}

int main(int argc, const char *argv[]) {
    const int screenWidth = 512;
    const int screenHeight = 256;

    std::cout << "P3\n" << screenWidth << " " << screenHeight << "\n255" << std::endl;

    const glm::vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    const glm::vec3 horizontal(4.0f, 0.0f, 0.0f);
    const glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    const glm::vec3 origin(0.0f, 0.0f, 0.0f);

    hitable *list[2];
    list[0] = new sphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f);
    list[1] = new sphere(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f);
    list[2] = new sphere(glm::vec3(-1.75f, 0.5f, -2.0f), 0.3f);
    hitable* world = new hitable_list(list, 3);

    for (int j = screenHeight-1; j >= 0; j--) {
        for (int i = 0; i < screenWidth; i++) {
            float u = float(i) / float(screenWidth);
            float v = float(j) / float(screenHeight);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            glm::vec3 col = color(r, world);

            int ir = int(255.99*col.x);
            int ig = int(255.99*col.y);
            int ib = int(255.99*col.z);

            std::cout << ir << " " << ig << " " << ib << std::endl;
        }
        
    }

    return 0;
}
