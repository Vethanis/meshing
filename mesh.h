#ifndef MESH_H
#define MESH_H

#include "vertexbuffer.h"

class Mesh{
public:
	unsigned vao, vbo, num_vertices;
	Mesh() : num_vertices(0){};
	void draw();
	void update(VertexBuffer& vb);
	void init();
	void destroy();
};

#endif
