
#include "myglheaders.h"
#include "mesh.h"
#include "debugmacro.h"

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
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 4, GL_INT, GL_FALSE, int(stride), (void*)offset);
}


void Mesh::init(){
    num_vertices = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    mesh_layout<glm::vec3>(0, sizeof(Vertex), 0);
    mesh_layout<glm::vec3>(1, sizeof(Vertex), sizeof(glm::vec3));
    mesh_layout<glm::vec3>(2, sizeof(Vertex), 2 * sizeof(glm::vec3));

    MYGLERRORMACRO
    is_init = true;
}

void Mesh::destroy(){
    if(!is_init){
        return;
    }
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    is_init = false;
    num_vertices = 0;
    MYGLERRORMACRO
}

void Mesh::update(VertexBuffer& vb){
    if(!is_init){
        this->init();
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vb.size(), vb.data(), GL_STATIC_DRAW);
    MYGLERRORMACRO
    num_vertices = unsigned(vb.size());
}

void Mesh::draw(){
    if(!num_vertices){return;}
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, num_vertices);
    MYGLERRORMACRO
}
