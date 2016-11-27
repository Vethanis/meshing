#include "mesh.h"
#include "debugmacro.h"

void Mesh::init(){
    num_vertices = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    mesh_layout<glm::vec3>(0, sizeof(Vertex), 0);
    mesh_layout<glm::vec3>(1, sizeof(Vertex), sizeof(glm::vec3));
    mesh_layout<int>(2, sizeof(Vertex), 2 * sizeof(glm::vec3));

    MYGLERRORMACRO
    is_init = true;
}

void Mesh::destroy(){
    if(!is_init)
        return;
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    MYGLERRORMACRO
}

void Mesh::update(VertexBuffer& vb){
    if(!is_init)
        this->init();
	if (!vb.size())
		return;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vb.size(), &vb[0], GL_STATIC_DRAW);
    MYGLERRORMACRO
    num_vertices = (unsigned)vb.size();
}

void Mesh::draw(){
    if(!num_vertices)return;
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, num_vertices);
    MYGLERRORMACRO
}
