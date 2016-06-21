#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

class Particle {
    public:
        //uses newton-euler integration method to move the particle forward in time by some amount
        void integrate(float duration);

        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 forceAccum;
        glm::vec3 acceleration;

        //required due to numerical instability
        float damping;

        float inverseMass;
    private:
    protected:
};

#endif /* end of include guard: PARTICLE_H */
