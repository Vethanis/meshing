#include "myglheaders.h"
#include "mesh.h"
#include "debugmacro.h"

void Mesh::init(){
	num_vertices = 0;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glEnableVertexAttribArray(0);	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	
	glEnableVertexAttribArray(1);	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3));
	
	glEnableVertexAttribArray(2);	// color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*6));
	
	glEnableVertexAttribArray(3);	// radius
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*9));
	
	MYGLERRORMACRO
}

void Mesh::destroy(){
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	MYGLERRORMACRO
}

void Mesh::update(VertexBuffer& vb){
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vb.size(), &vb[0], GL_STATIC_DRAW);
	MYGLERRORMACRO
	num_vertices = vb.size();
}

void Mesh::draw(){
	if(!num_vertices)return;
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, num_vertices);
	MYGLERRORMACRO
}

