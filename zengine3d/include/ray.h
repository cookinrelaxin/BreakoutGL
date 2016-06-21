#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class ray {
    public:
        /* Functions */
        ray();
        ray(const glm::vec3& a, const glm::vec3&b);
        glm::vec3 origin() const;
        glm::vec3 direction() const;
        glm::vec3 point_at_parameter(float t) const;
        /* Public Data */

    private:
        /* Private Data */
        glm::vec3 A;
        glm::vec3 B;
        /* Private Functions */
    protected:
        /* Inherited Data */
        /* Inherited Functions */
};


#endif /* end of include guard: RAY_H */
