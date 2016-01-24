#ifndef CSG_H
#define CSG_H

#include "glm/glm.hpp"
#include <cfloat>
#include <vector>

inline float sphere(const glm::vec3& p, const glm::vec3& b){
	return length(p) - b.x;
}

inline float box(const glm::vec3& p, const glm::vec3& b){
	glm::vec3 d = abs(p) - b;
	return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
}

typedef float (*SDF_Func)(const glm::vec3& c, const glm::vec3& p);

struct CSG{
	glm::vec3 center;
	glm::vec3 params;
	SDF_Func func;
	int material;
	CSG(const glm::vec3& c, const glm::vec3& p, SDF_Func f, int m)
		: center(c), params(p), func(f), material(m){};
	CSG(const CSG& o)
		: center(o.center), params(o.params), func(o.func), material(o.material){};
	inline CSG& operator = (const CSG& other){
		center = other.center;
		params = other.params;
		func = other.func;
		material = other.material;
		return *this;
	}
	inline float minX()const{
		if(func == &sphere || func == &box){
			return center.x - params.x;
		}
		return 0.0f;
	}
	inline float maxX()const{
		if(func == &sphere || func == &box){
			return center.x + params.x;
		}
		return 0.0f;
	}	
	inline float minY()const{
		if(func == &sphere){
			return center.y - params.x;
		}
		else if(func == &box){
			return center.y - params.y;
		}
		return 0.0f;
	}
	inline float maxY()const{
		if(func == &sphere){
			return center.y + params.x;
		}
		else if(func == &box){
			return center.y + params.y;
		}
		return 0.0f;
	}
	inline float minZ()const{
		if(func == &sphere){
			return center.z - params.x;
		}
		else if(func == &box){
			return center.z - params.z;
		}
		return 0.0f;
	}
	inline float maxZ()const{
		if(func == &sphere){
			return center.z + params.x;
		}
		else if(func == &box){
			return center.z + params.z;
		}
		return 0.0f;
	}
};

typedef std::vector<CSG> CSGList;

inline void getBounds(CSGList& list, glm::vec3& lo, glm::vec3& hi){
	lo = glm::vec3(FLT_MAX);
	hi = glm::vec3(FLT_MIN);
	SDF_Func sph = &sphere;
	for(auto& i : list){
		if(i.func == sph){
			lo = glm::min(lo, i.center - i.params.x);
			hi = glm::max(hi, i.center + i.params.x);
		}
		else{
			lo = glm::min(lo, i.center - i.params);
			hi = glm::min(hi, i.center + i.params);
		}
	}
}

inline float map(const glm::vec3& p, CSGList& list, int& material){
	float min = FLT_MAX;
	int mat = -1;
	for(auto& i : list){
		float f = i.func(p - i.center, i.params);
		if(f < min){
			min = f;
			mat = i.material;
		}
	}
	material = mat;
	return min;
}

inline float map(const glm::vec3& p, CSGList& list){
	float min = FLT_MAX;
	for(auto& i : list)
		min = glm::min(min, i.func(p - i.center, i.params));
	return min;
}

inline void fillCells(VertexBuffer& vb, CSGList& list, float spu){
	glm::vec3 dx(0.01f, 0.0f, 0.0f);
	glm::vec3 dy(0.0f, 0.01f, 0.0f);
	glm::vec3 dz(0.0f, 0.0f, 0.01f);
	glm::vec3 min, max;
	getBounds(list, min, max);
	float pitch = 1.0f / spu;
	float psize = 1024.0f * pitch;
	for(float z = min.z; z <= max.z; z += pitch){
		for(float y = min.y; y <= max.y; y += pitch){
			for(float x = min.x; x <= max.x; x += pitch){
				glm::vec3 i(x, y, z);
				if(map(i, list) > pitch)continue;
				for(int j = 0; j < 60; j++){
					float dis = map(i, list);
					glm::vec3 N = glm::normalize(glm::vec3(
						map(i+dx, list) - map(i-dx, list),
						map(i+dy, list) - map(i-dy, list),
						map(i+dz, list) - map(i-dz, list)));
					if(glm::abs(dis) < pitch*0.01f){
						int mat;
						map(i, list, mat);
						vb.push_back(Vertex(i, N, N*0.5f + 0.5f, psize));
						break;
					}
					i -= N * dis;
				}
			}
		}
	}
}

#endif
