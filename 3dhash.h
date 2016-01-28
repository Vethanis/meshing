#ifndef 3DHASH_H
#define 3DHASH_H

#include "glm/glm.hpp"
#include <vector>
#include <cmath>

template <typename T>
class 3DHashTable{
	glm::vec3 center; // center in world space
	float width; // half size of each dimension in world space. max = center + width, min = center - width
	int length; // number of cells along each dimension
	int items; // number of items currently held
	std::vector<T>* data; 
	
	inline std::vector<T>* operator()(const glm::vec3& v){
		glm::vec3 i(v - (center - width));
		int x = fmod(i.x, (float)length);
		int y = fmod(i.y, (float)length);
		int z = fmod(i.z, (float)length);
		return data + x + y*length + z*length*length;
	}
};

#endif
