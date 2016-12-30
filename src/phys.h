#pragma once

#include "glm/glm.hpp"
#include "resource.h"

struct phys_object{
    glm::vec3 pos, vel;
    float mass;
}

struct phys{
    Resource<phs_object> objects;
    void step(float dt){
        objects.for_each([dt](const T& item){
            item.pos += item.vel * dt;
        });
    }
}
