#ifndef CSG_H
#define CSG_H

#include "glm/glm.hpp"
#include <cfloat>
#include <vector>
#include <set>
#include "math.h"

inline float smin(float a, float b, float k){
    float h = glm::clamp( 0.5f+0.5f*(b-a)/k, 0.0f, 1.0f );
    return glm::mix( b, a, h ) - k*h*(1.0f-h);
}

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
    return glm::max(a, -b);
}
inline float blend_sadd(float a, float b, float r){
    return smin(a, b, r);
}

enum CSG_Type{
    SPHEREADD,
    SPHERESADD,
    SPHERESUB,
    BOXADD,
    BOXSADD,
    BOXSUB
};

struct CSG{
    glm::vec3 center, params;
    CSG_Type type;
    int material;
	CSG(const glm::vec3& c, const glm::vec3& p, CSG_Type t, int m) : center(c), params(p), type(t), material(m) {};
    float func(const glm::vec3& p)const{
		if (type == SPHEREADD || type == SPHERESADD || type == SPHERESUB) {
			return sphere_func(p - center, params.x);
		}
		return box_func(p - center, params);
    }
    glm::vec3 min()const{
		if (type == SPHEREADD || type == SPHERESADD || type == SPHERESUB) {
			return sphere_min(center, params.x);
		}
		return box_min(center, params);
    }
    glm::vec3 max()const{
		if (type == SPHEREADD || type == SPHERESADD || type == SPHERESUB) {
			return sphere_max(center, params.x);
		}
		return box_max(center, params);
    }
    float blend(float a, float b)const{
		if(type == BOXSADD || type == SPHERESADD)
			return blend_sadd(a, b, params.x * 0.5f);
		if(type == SPHEREADD || type == BOXADD)
			return blend_add(a, b);
		return blend_sub(a, b);
    }
};

typedef std::set<CSG*> CSGList;

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

inline maphit map(const glm::vec3& p, CSGList& list, float rad){
    maphit hit = {nullptr, rad * 1.732051f};
    for(CSG* i : list){
        float dis = i->blend(hit.distance, i->func(p));
        if(dis < hit.distance){
            hit.id = i;
            hit.distance = dis;
        }
    }
    return hit;
}

inline float map_raw(const glm::vec3& p, CSGList& list) {
	float dis = FLT_MAX;
	for (auto* i : list) {
		dis = glm::min(dis, fabsf(i->func(p)));
	}
	return dis;
}

inline glm::vec3 map_normal(const glm::vec3& p, CSGList& list){
    return glm::normalize(glm::vec3(
        map_raw(p + glm::vec3(0.001f, 0.0f, 0.0f), list) - map_raw(p - glm::vec3(0.001f, 0.0f, 0.0f), list),
		map_raw(p + glm::vec3(0.0f, 0.001f, 0.0f), list) - map_raw(p - glm::vec3(0.0f, 0.001f, 0.0f), list),
		map_raw(p + glm::vec3(0.0f, 0.0f, 0.001f), list) - map_raw(p - glm::vec3(0.0f, 0.0f, 0.001f), list)
    ));
}

static int fill_depth = 5;

inline void fillInd(VertexBuffer& vb, CSGList& list, CSGList& active_set, const glm::vec3& center, float radius, int depth){

    if(depth == fill_depth){
        glm::vec3 N = map_normal(center, list);
        glm::vec3 p = center;

        maphit mh = map(p, list, radius);
        p +=  N * mh.distance;
        if(mh.distance < 0.0f)return;
        vb.push_back({p, N, int(size_t(mh.id) % INT_MAX)});
        active_set.insert(mh.id);
        return;
    }

    const float hr = radius * 0.5f;
    for(char i = 0; i < 8; i++){
        glm::vec3 c(center);
        c.x += (i & 4) ? hr : -hr;
        c.y += (i & 2) ? hr : -hr;
        c.z += (i & 1) ? hr : -hr;
        maphit mh = map(c, list, hr);
        if(mh.id){
            fillInd(vb, list, active_set, c, hr, depth + 1);
        }
    }
}

inline void fillCells(VertexBuffer& vb, CSGList& list, const glm::vec3& center, float radius){
    if (!list.size())return;
    list.erase(nullptr);
    vb.clear();
    CSGList active_set;
    fillInd(vb, list, active_set, center, radius, 0);
    list.swap(active_set);
}

inline void fillCells(VertexBuffer& vb, CSG& item, const glm::vec3& center, float radius) {
	vb.clear();
	CSGList list, active_set;
	list.insert(&item);
	fillInd(vb, list, active_set, center, radius, 0);
}

#endif
