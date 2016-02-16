#ifndef CSG_H
#define CSG_H

#include "glm/glm.hpp"
#include <cfloat>
#include <vector>

float terrain(const glm::vec3& p);

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
		return glm::max(a, -b); 
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
		return glm::max(a, -b); 
	}
};

static SphereAdd SPHEREADD;
static SphereSAdd SPHERESADD;
static SphereSub SPHERESUB;
static BoxAdd BOXADD;
static BoxSAdd BOXSADD;
static BoxSub BOXSUB;

struct CSG{
	glm::vec3 center, params;
	SDF_Base* sdf;
	float r;
	int material;
	CSG(const glm::vec3& c, const glm::vec3& p, SDF_Base* sdf_obj, float rad=1.0f, int m=0)
		: center(c), params(p), sdf(sdf_obj), r(rad), material(m){};
	CSG(const CSG& o)
		: center(o.center), params(o.params), sdf(o.sdf), r(o.r), material(o.material) {};
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
		return sdf->func(p - center, params);
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
	for(CSG& i : list)
		min = i.blend(min, i.func(p));
	return min;
}

inline float map(const glm::vec3& p, std::vector<CSG*>& list){
	float min = FLT_MAX;
	for(CSG* i : list)
		min = i->blend(min, i->func(p));
	return min;
}

inline void fillCells(VertexBuffer& vb, std::vector<CSG*>& list, const glm::vec3& min, const glm::vec3& max, float spu){
	if(list.empty())return;
	glm::vec3 dx(0.001f, 0.0f, 0.0f);
	glm::vec3 dy(0.0f, 0.001f, 0.0f);
	glm::vec3 dz(0.0f, 0.0f, 0.001f);
	float pitch = 1.0f / spu;
	float p2 = pitch * 0.5f;
	float p3 = p2 * 0.5f;
	float psize = (1280.0f+720.0f) * 0.5f * p3;
	for(float z = min.z; z <= max.z; z += pitch){
	for(float y = min.y; y <= max.y; y += pitch){
	for(float x = min.x; x <= max.x; ){
		glm::vec3 i(x, y, z);
		float dis = map(i, list);
		if(glm::abs(dis) < p2){
			for(float z2 = i.z - p2; z2 <= i.z + p2; z2 += p2){
			for(float y2 = i.y - p2; y2 <= i.y + p2; y2 += p2){
			for(float x2 = i.x - p2; x2 <= i.x + p2;){
				glm::vec3 k(x2, y2, z2);
				float d2 = map(k, list);
				if(glm::abs(d2) < p3){
					for(float z3 = k.z - p3; z3 <= k.z + p3; z3 += p3){
					for(float y3 = k.y - p3; y3 <= k.y + p3; y3 += p3){
					for(float x3 = k.x - p3; x3 <= k.x + p3;){
						glm::vec3 j(x3, y3, z3);
						float d3 = map(j, list);
						if(glm::abs(d3) < p3*0.5f){
							for(int l = 0; l < 30; l++){
								d3 = map(j, list);
								glm::vec3 N = glm::normalize(glm::vec3(
									map(j+dx, list) - map(j-dx, list),
									map(j+dy, list) - map(j-dy, list),
									map(j+dz, list) - map(j-dz, list)));
								if(glm::abs(d3) < p3*0.25f){
									vb.push_back(Vertex(j, N, N*0.5f + 0.5f, psize));
									break;
								}
								j -= N * d3 * 0.75f;
							}
						}
						x3 += glm::max(d3, p3);
					}}}
				}
				x2 += glm::max(d2, p2);
			}}}
		}
		x += glm::max(pitch, dis);
	}}}
}

inline void fillCells(VertexBuffer& vb, CSGList& list, const glm::vec3& min, const glm::vec3& max, float spu){
	if(list.empty())return;
	glm::vec3 dx(0.001f, 0.0f, 0.0f);
	glm::vec3 dy(0.0f, 0.001f, 0.0f);
	glm::vec3 dz(0.0f, 0.0f, 0.001f);
	float pitch = 1.0f / spu;
	float p2 = pitch * 0.5f;
	float p3 = p2 * 0.5f;
	float psize = (1280.0f+720.0f) * 0.5f * p3;
	for(float z = min.z; z <= max.z; z += pitch){
	for(float y = min.y; y <= max.y; y += pitch){
	for(float x = min.x; x <= max.x; ){
		glm::vec3 i(x, y, z);
		float dis = map(i, list);
		if(glm::abs(dis) < p2){
			for(float z2 = i.z - p2; z2 <= i.z + p2; z2 += p2){
			for(float y2 = i.y - p2; y2 <= i.y + p2; y2 += p2){
			for(float x2 = i.x - p2; x2 <= i.x + p2;){
				glm::vec3 k(x2, y2, z2);
				float d2 = map(k, list);
				if(glm::abs(d2) < p3){
					for(float z3 = k.z - p3; z3 <= k.z + p3; z3 += p3){
					for(float y3 = k.y - p3; y3 <= k.y + p3; y3 += p3){
					for(float x3 = k.x - p3; x3 <= k.x + p3;){
						glm::vec3 j(x3, y3, z3);
						float d3 = map(j, list);
						if(glm::abs(d3) < p3*0.5f){
							for(int l = 0; l < 30; l++){
								d3 = map(j, list);
								glm::vec3 N = glm::normalize(glm::vec3(
									map(j+dx, list) - map(j-dx, list),
									map(j+dy, list) - map(j-dy, list),
									map(j+dz, list) - map(j-dz, list)));
								if(glm::abs(d3) < p3*0.25f){
									vb.push_back(Vertex(j, N, N*0.5f + 0.5f, psize));
									break;
								}
								j -= N * d3 * 0.75f;
							}
						}
						x3 += glm::max(d3, p3);
					}}}
				}
				x2 += glm::max(d2, p2);
			}}}
		}
		x += glm::max(pitch, dis);
	}}}
}

inline void fillCells(VertexBuffer& vb, CSGList& list, float spu){
	glm::vec3 min, max;
	getBounds(list, min, max);
	fillCells(vb, list, min, max, spu);
}

inline void fillCells(VertexBuffer& vb, const CSG& item, float spu){
	glm::vec3 dx(0.01f, 0.0f, 0.0f);
	glm::vec3 dy(0.0f, 0.01f, 0.0f);
	glm::vec3 dz(0.0f, 0.0f, 0.01f);
	glm::vec3 min, max;
	min = item.min();
	max = item.max();
	spu = 4.0f * spu;
	float pitch = 1.0f / spu;
	float psize = (1280.0f+720.0f) * 0.5f * pitch;
	for(float z = min.z; z <= max.z; z += pitch){
	for(float y = min.y; y <= max.y; y += pitch){
	for(float x = min.x; x <= max.x; ){
		glm::vec3 i(x, y, z);
		float dis = item.func(i);
		if(dis < pitch){
			for(int j = 0; j < 30; j++){
				dis = item.func(i);
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
		}
		x += glm::max(dis, pitch);
	}}}
}

#endif
