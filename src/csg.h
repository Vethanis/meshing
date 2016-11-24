#ifndef CSG_H
#define CSG_H

#include "glm/glm.hpp"
#include <cfloat>
#include <vector>
#include <algorithm>
#include <set>
#include "math.h"

inline float sphere_func(const glm::vec3& p, float r){
    return glm::length(p) - r;
}
inline float box_func(const glm::vec3& p, const glm::vec3& b){
    glm::vec3 d = abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
}

inline glm::vec3 sphere_min(const glm::vec3& c, float r){
    return c - r;
}
inline glm::vec3 sphere_max(const glm::vec3& c, float r){
    return c + r;
}
inline glm::vec3 box_min(const glm::vec3& c, const glm::vec3& p){
    return c - p;
}
inline glm::vec3 box_max(const glm::vec3& c, const glm::vec3& p){
    return c + p;
}
inline float blend_add(float a, float b){
    return glm::min(a, b);
}
inline float blend_sub(float a, float b){
    return glm::max(-b, a);
}
inline float blend_sadd(float a, float b, float r){
    float e = glm::max(r - fabsf(a - b), 0.0f);
	return glm::min(a, b) - e*e*0.25f/r;
}
// box: type & 1 == 0
// sphere: type & 1 == 1
// add: type & 2 == 0
// sub: type & 2 == 2
// sadd: type & 4 == 4
enum CSG_Type{
    BOXADD = 0,
    SPHEREADD = 1,
    BOXSUB = 2,
    SPHERESUB = 3,
    BOXSADD = 4,
    SPHERESADD = 5
};

struct CSG{
    glm::vec3 center, params;
    CSG_Type type;
    int material;
	CSG(const glm::vec3& c, const glm::vec3& p, CSG_Type t, int m) : center(c), params(p), type(t), material(m) {};
    inline float func(const glm::vec3& p)const{
		if (type & 1) {
			return sphere_func(p - center, params.x);
		}
		return box_func(p - center, params);
    }
    inline float blend(float a, float b)const{
		if(type & 4)
			return blend_sadd(a, b, params.x * 0.5f);
		if(type & 2)
			return blend_sub(a, b);
		return blend_add(a, b);
    }
};

typedef std::vector<CSG*> CSGList;

struct maphit{
    CSG* id;
    float distance;
};

inline float operator-(const maphit& a, const maphit& b){
    return a.distance - b.distance;
}
inline float operator-(const float a, const maphit& b){
    return a - b.distance;
}
inline float operator-(const maphit& a, const float b){
    return a.distance - b;
}

inline maphit map(const glm::vec3& p, CSGList& list){
    maphit hit = {nullptr, FLT_MAX};
    for(auto* i : list){
        float dis = i->blend(hit.distance, i->func(p));
        if(dis < hit.distance){
            hit.id = i;
            hit.distance = dis;
        }
    }
    return hit;
}

inline glm::vec3 map_normal(const glm::vec3& p, CSGList& list){
    return glm::normalize(glm::vec3(
        map(p + glm::vec3(0.001f, 0.0f, 0.0f), list) - map(p - glm::vec3(0.001f, 0.0f, 0.0f), list),
		map(p + glm::vec3(0.0f, 0.001f, 0.0f), list) - map(p - glm::vec3(0.0f, 0.001f, 0.0f), list),
		map(p + glm::vec3(0.0f, 0.0f, 0.001f), list) - map(p - glm::vec3(0.0f, 0.0f, 0.001f), list)
    ));
}

static int fill_depth = 5;

inline void fillInd(VertexBuffer& vb, CSGList& list, const glm::vec3& center, float radius, int depth){

    if(depth == fill_depth){
        glm::vec3 N = map_normal(center, list);
        glm::vec3 p = center;

        maphit mh = map(p, list);
        if(!mh.id || mh.distance < 0.0f)return;
        p +=  N * mh.distance;
        vb.push_back({p, N, mh.id->material});
        return;
    }

    const float hr = radius * 0.5f;
    for(char i = 0; i < 8; i++){
        glm::vec3 c(center);
        c.x += (i & 4) ? hr : -hr;
        c.y += (i & 2) ? hr : -hr;
        c.z += (i & 1) ? hr : -hr;
        maphit mh = map(c, list);
        if(mh.id && fabsf(mh.distance) < hr * 1.732051f){
            fillInd(vb, list, c, hr, depth + 1);
        }
    }
}

inline void fillCells(VertexBuffer& vb, CSGList& list, const glm::vec3& center, float radius){
    if (!list.size())return;
    vb.clear();
    fillInd(vb, list, center, radius, 0);
}

inline void fillCells(VertexBuffer& vb, CSG& item, const glm::vec3& center, float radius) {
	vb.clear();
	CSGList list;
	list.push_back(&item);
	fillInd(vb, list, center, radius, -1);
}

#endif
