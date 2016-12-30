#ifndef CSG_H
#define CSG_H

#include "glm/glm.hpp"
#include <cfloat>
#include <vector>
#include "math.h"
#include "vertexbuffer.h"

struct CSG;

struct maphit{
    float distance;
    int id;
};

inline float operator-(const maphit& a, const maphit b){
    return a.distance - b.distance;
}
inline float operator-(const float a, const maphit b){
    return a - b.distance;
}
inline float operator-(const maphit a, const float b){
    return a.distance - b;
}

inline float sphere_func(const glm::vec3& p, float r){
    return glm::length(p) - r;
}
inline float box_func(const glm::vec3& p, const glm::vec3& b){
    glm::vec3 d = glm::abs(p) - b;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, glm::vec3(0.0f)));
}
inline maphit blend_add(maphit a, maphit b){
    return (a.distance < b.distance) ? a : b;
}
inline maphit blend_sub(maphit a, maphit b){
    b.distance = -b.distance;
    return (a.distance > b.distance) ? a : b;
}
inline maphit blend_sadd(maphit a, maphit b, float r){
    float e = glm::max(r - fabsf(a.distance - b.distance), 0.0f);
	float dis = glm::min(a.distance, b.distance) - e*e*0.25f/r;
    if(fabsf(dis - a) < fabsf(dis - b))
        return {dis, a.id};
    return {dis, b.id};
}
inline maphit blend_ssub(maphit a, maphit b, float r){
    a.distance = -a.distance;
    maphit m = blend_sadd(a, b, r);
    m.distance = -m.distance;
    return m;
}
// box: type & 1 == 0
// sphere: type & 1 == 1
// add: type & 2 == 0
// sub: type & 2 == 2
// sadd: type & 4 == 4
// ssub: type & 8 == 8
enum CSG_Type{
    BOXADD = 0,
    SPHEREADD = 1,
    BOXSUB = 2,
    SPHERESUB = 3,
    BOXSADD = 4,
    SPHERESADD = 5,
    BOXSSUB = 8,
    SPHERESSUB = 9
};

struct CSG{
    glm::vec3 center, params;
    CSG_Type type;
    int material;
	CSG(const glm::vec3& c, const glm::vec3& p, CSG_Type t, int m) : center(c), params(p), type(t), material(m) {};
    inline float func(const glm::vec3& p){
		if (type & 1) {
			return sphere_func(p - center, params.x);
		}
		return box_func(p - center, params);
    }
    inline maphit blend(maphit a, const glm::vec3& p){
        maphit b = {this->func(p), material};
		if(type & 4)
			return blend_sadd(a, b, glm::min(params.x * 0.5f, 0.2f));
        if(type & 8)
            return blend_ssub(a, b, glm::min(params.x * 0.5f, 0.2f));
		if(type & 2)
			return blend_sub(a, b);
		return blend_add(a, b);
    }
};

typedef std::vector<CSG*> CSGList;

inline maphit map(const glm::vec3& p, CSGList& list){
    maphit hit = {FLT_MAX, 0};
    for(auto* i : list){
        hit = i->blend(hit, p);
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
    maphit mh = map(center, list);
    if(!mh.id || fabsf(mh.distance) > radius * 1.732051f)
        return;

    if(depth == fill_depth){
        glm::vec3 N = map_normal(center, list);
        vb.push_back({center - N * mh.distance, N, mh.id});
        return;
    }

    const float hr = radius * 0.5f;
    for(char i = 0; i < 8; i++){
        glm::vec3 c(center);
        c.x += (i & 4) ? hr : -hr;
        c.y += (i & 2) ? hr : -hr;
        c.z += (i & 1) ? hr : -hr;
        fillInd(vb, list, c, hr, depth + 1);
    }
}

inline void fillCells(VertexBuffer& vb, CSGList& list, const glm::vec3& center, float radius){
    if (!list.size()){
        return;
    }
    fillInd(vb, list, center, radius, 0);
}

inline void fillCells(VertexBuffer& vb, CSG& item, const glm::vec3& center, float radius) {
	CSGList list;
	list.push_back(&item);
	fillInd(vb, list, center, radius, 0);
}

#endif
