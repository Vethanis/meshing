#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "glm/glm.hpp"
#include <vector>

struct Vertex{
    glm::vec3 position, normal;
    int id;
};

typedef std::vector<Vertex> VertexBuffer;

#endif
