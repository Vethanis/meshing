#pragma once

#include "glm/glm.hpp"
#include <cfloat>
#include "math.h"
#include "vertexbuffer.h"
#include "ints.h"
#include <cstdio>
#include "array.h"

struct CSG_Param {
    glm::vec3 center, size, color;
    float smoothness;
    float roughness;
    float metalness;
    CSG_Param(){
        center = glm::vec3(0.0f);
        size = glm::vec3(0.1f);
        color = glm::vec3(0.5f);
        smoothness = 0.01f;
        roughness = 0.5f;
        metalness = 0.5f;
    }
};

struct maphit {
    u32 id;
    float distance;
};

inline float operator-(maphit a, maphit b){
    return a.distance - b.distance;
}
inline float operator-(float a, maphit b){
    return a - b.distance;
}
inline float operator-(maphit a, float b){
    return a.distance - b;
}

typedef float (*sdf_func)(const glm::vec3&, const CSG_Param&);
typedef maphit (*sdf_blend_func)(maphit, maphit, const CSG_Param&);

inline float sphere_func(const glm::vec3& p, const CSG_Param& param){
    return glm::distance(p, param.center) - param.size.x;
}
inline float box_func(const glm::vec3& p, const CSG_Param& param){
    glm::vec3 d = glm::abs(p - param.center) - param.size;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, glm::vec3(0.0f)));
}
inline maphit blend_add(maphit a, maphit b, const CSG_Param& param){
    return (a.distance < b.distance) ? a : b;
}
inline maphit blend_sub(maphit a, maphit b, const CSG_Param& param){
    b.distance = -b.distance;
    return (a.distance > b.distance) ? a : b;
}
inline maphit blend_sadd(maphit a, maphit b, const CSG_Param& param){
    float e = glm::max(param.smoothness - glm::abs(a.distance - b.distance), 0.0f);
	float dis = glm::min(a.distance, b.distance) - e*e*0.25f/param.smoothness;
    return {
        a.distance < b.distance ? a.id : b.id, 
        dis
    };
}
inline maphit blend_ssub(maphit a, maphit b, const CSG_Param& param){
    a.distance = -a.distance;
    maphit m = blend_sadd(a, b, param);
    m.distance = -m.distance;
    return m;
}

enum shape_t : u8 {
    SPHERE = 0,
    BOX = 1,
};

enum blend_t : u8 {
    ADD = 0,
    SUB = 1,
    SMOOTH_ADD = 2,
    SMOOTH_SUB = 3,
};

struct CSG_Type {
    u8 shape;
    u8 blend;
    CSG_Type() : shape(SPHERE), blend(ADD){};
};

struct CSG {
    CSG_Param param;
    CSG_Type type;
    float func(const glm::vec3& p) const {
        switch(type.shape){
            case SPHERE:
                return sphere_func(p, param);
            case BOX:
                return box_func(p, param);
        }
        return FLT_MAX;
    }
    maphit blend(maphit a, maphit b) const {
        switch(type.blend){
            case ADD:
                return blend_add(a, b, param);
            case SUB: 
                return blend_sub(a, b, param);
            case SMOOTH_ADD:
                return blend_sadd(a, b, param);
            case SMOOTH_SUB:
                return blend_ssub(a, b, param);
        }
        return a;
    }
    glm::vec3 normal(const glm::vec3& p)const{
        constexpr float e = 0.001f;
        return normalize(glm::vec3(
            func(p + glm::vec3(e, 0.0f, 0.0f)) - func(p - glm::vec3(e, 0.0f, 0.0f)),
            func(p + glm::vec3(0.0f, e, 0.0f)) - func(p - glm::vec3(0.0f, e, 0.0f)),
            func(p + glm::vec3(0.0f, 0.0f, e)) - func(p - glm::vec3(0.0f, 0.0f, e))
        ));
    }
};

typedef Array<CSG, 65536> CSGSet;

class CSGIndices {
    Array<u32, 512> indices;
public:
    bool push_back(u32 id){
        if(indices.full()){
            return false;
        }
        if(indices.find(id) != -1)
            return false;

        indices.grow() = id;

        return true;
    }
    void clear(){
        indices.clear();
    }
    s32 count() const { return indices.count(); }
    maphit map(const glm::vec3& p, const CSGSet& set) const {
        maphit a = {u32(-1), FLT_MAX};
        for(u32 i : indices){
            const CSG& csg = set[i];
            maphit b = { 
                i, 
                csg.func(p) 
            };
            a = csg.blend(a, b);
        }
        return a;
    }
    glm::vec3 map_normal(const glm::vec3& p, const CSGSet& set) const {
        constexpr float e = 0.001f;
        return glm::normalize(glm::vec3(
            map(p + glm::vec3(e, 0.0f, 0.0f), set) - map(p - glm::vec3(e, 0.0f, 0.0f), set),
            map(p + glm::vec3(0.0f, e, 0.0f), set) - map(p - glm::vec3(0.0f, e, 0.0f), set),
            map(p + glm::vec3(0.0f, 0.0f, e), set) - map(p - glm::vec3(0.0f, 0.0f, e), set)
        ));
    }
};

inline void fillInd(VertexBuffer& vb, const CSGSet& set, const CSGIndices& list, const glm::vec3& center, float radius, int depth){
    constexpr int fill_depth = 5;

    maphit mh = list.map(center, set);
    if(fabsf(mh.distance) > radius * 1.732051f)
        return;

    if(depth == fill_depth){
        glm::vec3 N = list.map_normal(center, set);
        vb.grow() = { 
            center - N * mh.distance, 
            N, 
            set[mh.id].param.color,
            set[mh.id].param.roughness,
            set[mh.id].param.metalness
        };
        return;
    }

    const float hr = radius * 0.5f;
    for(int i = 0; i < 8; i++){
        glm::vec3 c(center);
        c.x += (i & 4) ? hr : -hr;
        c.y += (i & 2) ? hr : -hr;
        c.z += (i & 1) ? hr : -hr;
        fillInd(vb, set, list, c, hr, depth + 1);
    }
}

inline void fillCells(VertexBuffer& vb, const CSGSet& set, const CSGIndices& list, const glm::vec3& center, float radius){
    vb.clear();
    if (!list.count()){
        return;
    }
    fillInd(vb, set, list, center, radius, 0);
}

inline void fillInd(VertexBuffer& vb, const CSG& item, const glm::vec3& center, float radius, int depth){
    constexpr int fill_depth = 5;

    float dis = item.func(center);
    if(fabsf(dis) > radius * 1.732051f)
        return;

    if(depth == fill_depth){
        glm::vec3 N = item.normal(center);
        vb.grow() = { 
            center - N * dis, 
            N, 
            item.param.color,
            item.param.roughness,
            item.param.metalness
        };
        return;
    }

    const float hr = radius * 0.5f;
    for(int i = 0; i < 8; i++){
        glm::vec3 c(center);
        c.x += (i & 4) ? hr : -hr;
        c.y += (i & 2) ? hr : -hr;
        c.z += (i & 1) ? hr : -hr;
        fillInd(vb, item, c, hr, depth + 1);
    }
}

inline void fillCells( VertexBuffer& vb, const CSG& item, const glm::vec3& center, float radius) {
    vb.clear();
	fillInd(vb, item, center, radius, 0);
}
