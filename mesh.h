#ifndef MESH_H
#define MESH_H

#include "vertexbuffer.h"

class Mesh{
	unsigned vao, vbo, num_vertices;
public:
	Mesh();
	~Mesh();
	void draw();
	void upload(const VertexBuffer& vbuf);
};

#endif
