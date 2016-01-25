#ifndef CSG_H
#define CSG_H

#include "glm/glm.hpp"
#include <cfloat>
#include <vector>

struct SDF_Base{
	virtual float func(const glm::vec3& c, const glm::vec3& p)=0;
	virtual glm::vec3 min(const glm::vec3& c, const glm::vec3& p)=0;
	virtual glm::vec3 max(const glm::vec3& c, const glm::vec3& p)=0;
	virtual float blend(float a, float b, float r)=0;
};

inline float smin(float a, float b, float k){
    float h = glm::clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return glm::mix( b, a, h ) - k*h*(1.0-h);
}
inline float smax(float a, float b, float r){
	return smin(a, -b, r);
}

struct SphereAdd : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		return length(p) - b.x;
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p.x;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p.x;
	}
	float blend(float a, float b, float r){ 
		return glm::min(a, b); 
	}
};

struct SphereSAdd : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		return length(p) - b.x;
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p.x;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p.x;
	}
	float blend(float a, float b, float r){ 
		return smin(a, b, r); 
	}
};

struct SphereSub : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		return length(p) - b.x;
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p.x;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p.x;
	}
	float blend(float a, float b, float r){ 
		return glm::max(a, b); 
	}
};

struct SphereSSub : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		return length(p) - b.x;
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p.x;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p.x;
	}
	float blend(float a, float b, float r){ 
		return smax(a, b, r); 
	}
};

struct BoxAdd : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		glm::vec3 d = abs(p) - b;
		return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p;
	}
	float blend(float a, float b, float r){ 
		return glm::min(a, b); 
	}
};

struct BoxSAdd : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		glm::vec3 d = abs(p) - b;
		return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p;
	}
	float blend(float a, float b, float r){ 
		return smin(a, b, r); 
	}
};

struct BoxSub : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		glm::vec3 d = abs(p) - b;
		return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p;
	}
	float blend(float a, float b, float r){ 
		return glm::max(a, b); 
	}
};

struct BoxSSub : public SDF_Base{
	float func(const glm::vec3& p, const glm::vec3& b){
		glm::vec3 d = abs(p) - b;
		return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
	}
	glm::vec3 min(const glm::vec3& c, const glm::vec3& p){
		return c - p;
	}
	glm::vec3 max(const glm::vec3& c, const glm::vec3& p){
		return c + p;
	}
	float blend(float a, float b, float r){ 
		return smax(a, b, r); 
	}
};

static SphereAdd SPHEREADD;
static SphereSAdd SPHERESADD;
static SphereSub SPHERESUB;
static SphereSSub SPHERESSUB;
static BoxAdd BOXADD;
static BoxSAdd BOXSADD;
static BoxSub BOXSUB;
static BoxSSub BOXSSUB;

struct CSG{
	glm::vec3 center, params;
	SDF_Base* sdf;
	float r;
	int material;
	CSG(const glm::vec3& c, const glm::vec3& p, SDF_Base* sdf_obj, float rad=1.0f, int m=0)
		: center(c), params(p), sdf(sdf_obj), r(rad), material(m){};
	CSG(const CSG& o)
		: center(o.center), params(o.params), sdf(o.sdf), r(o.r), material(o.material){};
	~CSG(){};
	inline CSG& operator = (const CSG& other){
		center = other.center;
		params = other.params;
		sdf = other.sdf;
		r = other.r;
		material = other.material;
		return *this;
	}
	float func(const glm::vec3& p)const{
		return sdf->func(p-center, params);
	}
	glm::vec3 min()const{
		return sdf->min(center, params);
	}
	glm::vec3 max()const{
		return sdf->max(center, params);
	}
	float blend(float a, float b)const{
		return sdf->blend(a, b, r);
	}
};

typedef std::vector<CSG> CSGList;

inline void getBounds(CSGList& list, glm::vec3& lo, glm::vec3& hi){
	lo = glm::vec3(FLT_MAX);
	hi = glm::vec3(FLT_MIN);
	for(auto& i : list){
		lo = glm::min(lo, i.min());
		hi = glm::max(hi, i.max());
	}
}

inline float map(const glm::vec3& p, CSGList& list){
	float min = FLT_MAX;
	for(auto& i : list)
		min = i.blend(min, i.func(p));
	return min;
}

inline void fillCells(VertexBuffer& vb, CSGList& list, const glm::vec3& min, const glm::vec3& max, float spu){
	if(list.empty())return;
	glm::vec3 dx(0.01f, 0.0f, 0.0f);
	glm::vec3 dy(0.0f, 0.01f, 0.0f);
	glm::vec3 dz(0.0f, 0.0f, 0.01f);
	float pitch = 1.0f / spu;
	float psize = 1024 * pitch;
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
				vb.push_back(Vertex(i, N, N*0.5f + 0.5f, psize));
				break;
			}
			i -= N * dis;
		}
	}}}
}

inline void fillCells(VertexBuffer& vb, const CSG& item, float spu){
	glm::vec3 dx(0.1f, 0.0f, 0.0f);
	glm::vec3 dy(0.0f, 0.1f, 0.0f);
	glm::vec3 dz(0.0f, 0.0f, 0.1f);
	glm::vec3 min, max;
	min = item.min();
	max = item.max();
	float pitch = 1.0f / spu;
	float psize = 1024 * pitch;
	for(float z = min.z; z <= max.z; z += pitch){
	for(float y = min.y; y <= max.y; y += pitch){
	for(float x = min.x; x <= max.x; x += pitch){
		glm::vec3 i(x, y, z);
		if(item.func(i) > pitch)continue;
		for(int j = 0; j < 60; j++){
			float dis = item.func(i);
			glm::vec3 N = glm::normalize(glm::vec3(
				item.func(i+dx) - item.func(i-dx),
				item.func(i+dy) - item.func(i-dy),
				item.func(i+dz) - item.func(i-dz)));
			if(glm::abs(dis) < pitch*0.01f){
				vb.push_back(Vertex(i, N, glm::vec3(0.0f, 1.0f, 0.0f), psize));
				break;
			}
			i -= N * dis;
		}
	}}}
}

#endif
