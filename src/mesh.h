#ifndef MESH_H
#define MESH_H

#include "vertexbuffer.h"

class Mesh{
public:
    unsigned vao, vbo, num_vertices;
    bool is_init;
    Mesh() : num_vertices(0), is_init(false){};
    void draw();
    void update(VertexBuffer& vb);
    void init();
    void destroy();
};

#endif
