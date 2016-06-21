#include <particle.h>

void Particle::integrate(float duration) {
   assert(duration > 0.0); 

   position += velocity * duration;

   glm::vec3 resultingAcc = acceleration + forceAccum * inverseMass;

   velocity += resultingAcc * duration;

   velocity *= powf(damping, duration);
}
