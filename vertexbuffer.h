#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "glm/glm.hpp"
#include <vector>

struct Vertex{
	glm::vec3 position, normal, color;
	float radius;
	Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec3& c, float r)
		: position(p), normal(n), color(c), radius(r){};
	Vertex(const Vertex& o)
		: position(o.position), normal(o.normal), color(o.color), radius(o.radius){};
	Vertex& operator=(const Vertex& o){
		position = o.position; normal = o.normal; color = o.color; radius = o.radius;
		return *this;
	}
};

typedef std::vector<Vertex> VertexBuffer;

#endif
