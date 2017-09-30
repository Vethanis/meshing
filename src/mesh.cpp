
#include "myglheaders.h"
#include "mesh.h"
#include "debugmacro.h"

static size_t layout_stride;
static size_t layout_offset;

template<typename T>
void begin_mesh_layout(){
    layout_stride = sizeof(T);
    layout_offset = 0;
}

template<typename T>
void mesh_layout(int location);

template<>
inline void mesh_layout<float>(int location){
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 1, GL_FLOAT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(float);
}
template<>
inline void mesh_layout<glm::vec2>(int location){
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(glm::vec2);
}
template<>
inline void mesh_layout<glm::vec3>(int location){
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(glm::vec3);
}
template<>
inline void mesh_layout<glm::vec4>(int location){
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(glm::vec4);
}

template<>
inline void mesh_layout<int>(int location){
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 1, GL_INT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(int);
}
template<>
inline void mesh_layout<glm::ivec2>(int location){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 2, GL_INT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(glm::ivec2);
}
template<>
inline void mesh_layout<glm::ivec3>(int location){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 3, GL_INT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(glm::ivec3);
}
template<>
inline void mesh_layout<glm::ivec4>(int location){
    glEnableVertexAttribArray(location);    // position
    glVertexAttribPointer(location, 4, GL_INT, GL_FALSE, int(layout_stride), (void*)layout_offset);
    layout_offset += sizeof(glm::ivec4);
}


void Mesh::init(){
    num_vertices = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    begin_mesh_layout<Vertex>();
    mesh_layout<glm::vec3>(0);
    mesh_layout<glm::vec3>(1);
    mesh_layout<glm::vec3>(2);
    mesh_layout<float>(3);
    mesh_layout<float>(4);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vb.count(), vb.begin(), GL_STATIC_DRAW);
    MYGLERRORMACRO
    num_vertices = unsigned(vb.count());
}

void Mesh::draw(){
    if(!num_vertices){return;}
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, num_vertices);
    MYGLERRORMACRO
}
