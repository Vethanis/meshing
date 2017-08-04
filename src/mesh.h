#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "vertexbuffer.h"

class Mesh {
    unsigned vao, vbo, num_vertices;
    bool is_init;
public:
    Mesh() : num_vertices(0), is_init(false){};
    ~Mesh(){ destroy(); }
    void draw();
    void update(VertexBuffer& vb);
    void init();
    void destroy();
};

#endif
