#pragma once

#include "glm/glm.hpp"
#include "resource.h"

struct AABB{
    glm::vec3 min, max;
};

struct phys_object{
    glm::vec3 pos, vel;
    float mass;
}

struct PhysicsManager{
    ThreadedResource<phys_object> objects;
    void step(float dt){
        objects.for_each_mut([dt](const T& item){
            item.pos += item.vel * dt;
        });
    }
}
