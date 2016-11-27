#ifndef MESH_H
#define MESH_H

#include "myglheaders.h"
#include "glm/glm.hpp"
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

template<typename T>
void mesh_layout(int location, size_t stride, size_t offset);

template<>
inline void mesh_layout<float>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 1, GL_FLOAT, GL_FALSE, int(stride), (void*)offset);
}
template<>
inline void mesh_layout<glm::vec2>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, int(stride), (void*)offset);
}
template<>
inline void mesh_layout<glm::vec3>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, int(stride), (void*)offset);
}
template<>
inline void mesh_layout<glm::vec4>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, int(stride), (void*)offset);
}

template<>
inline void mesh_layout<int>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 1, GL_INT, GL_FALSE, int(stride), (void*)offset);
}
template<>
inline void mesh_layout<glm::ivec2>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 2, GL_INT, GL_FALSE, int(stride), (void*)offset);
}
template<>
inline void mesh_layout<glm::ivec3>(int location, size_t stride, size_t offset){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 3, GL_INT, GL_FALSE, int(stride), (void*)offset);
}
template<>
inline void mesh_layout<glm::ivec4>(int location, size_t stride, size_t offset){
}

#endif
