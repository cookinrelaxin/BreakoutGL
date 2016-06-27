#include "dialectric.h"
#include <iostream>

bool dialectric::scatter(const ray& r_in,
                         const hit_record& rec,
                         glm::vec3& attenuation,
                         ray& scattered) const {
    glm::vec3 outward_normal;
    glm::vec3 reflected = this->reflect(r_in.direction(), rec.normal);
    float ni_over_nt;
    attenuation = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 refracted;
    float reflect_prob;
    float cosine;

    if (glm::dot(r_in.direction(), rec.normal) > 0.0f) {
       outward_normal = -rec.normal; 
       ni_over_nt = ref_idx;
       cosine = ref_idx * glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());
    }
    else {
       outward_normal = rec.normal; 
       ni_over_nt = 1.0f / ref_idx;
       cosine = -glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());
    }
    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine);
    }
    else {
        scattered = ray(rec.p, reflected);
        reflect_prob = 1.0f;
    }
    //std::cout << "cosine: " << cosine << std::endl;
    //std::cout << "reflect prob: " << reflect_prob << std::endl;
    if (drand48() < reflect_prob) {
       scattered = ray(rec.p, reflected); 
    }
    else {
       scattered = ray(rec.p, refracted); 
    }
    return true;
}

bool dialectric::refract(const glm::vec3& v,
                         const glm::vec3& n,
                         const float ni_over_nt,
                         glm::vec3& refracted) const {
    glm::vec3 uv = glm::normalize(v);
    float dt = dot(uv, n);
    float discriminant = 1.0f - ni_over_nt*ni_over_nt*(1.0f - dt*dt);
    if (discriminant > 0.0f) {
       refracted = ni_over_nt*(uv - n*dt) - n * (float)sqrt(discriminant); 
       return true;
    }
    else {
       return false; 
    }
}

float dialectric::schlick(const float cosine) const {
    float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
    r0 = r0*r0;
    return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}
