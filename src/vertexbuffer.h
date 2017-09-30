#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "glm/glm.hpp"
#include "array.h"

struct Vertex{
    glm::vec3 position, normal, color;
    float roughness;
    float metalness;
};

typedef Vector<Vertex> VertexBuffer;

#endif
