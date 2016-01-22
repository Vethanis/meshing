#ifndef OCTREE_H
#define OCTREE_H

#include "debugmacro.h"
#include "glm/glm.hpp"
#include <vector>
#include <algorithm>
#include "point.h"

typedef Point DataType;

struct OctNode{
    glm::vec3 center;
    std::vector<OctNode> children;
    DataType* data;
    float half_len;
    OctNode(const glm::vec3& _center, float _half_len)
    : center(_center), data(nullptr), half_len(_half_len){};
    ~OctNode(){ delete data; };
    inline unsigned char getChildIdx(glm::vec3& location){
        unsigned char i = 0;
        i |= location.x >= center.x ? 4 : 0;
        i |= location.y >= center.y ? 2 : 0;
        i |= location.z >= center.z ? 1 : 0;
        return i;
    }
    inline void makeChildren(){
        children.reserve(8);
        float q_len = half_len * 0.5f;
        for(unsigned char i=0; i<8; i++){
            glm::vec3 n_c(center);
            n_c.x += i&4 ? q_len : -q_len;
            n_c.y += i&2 ? q_len : -q_len;
            n_c.z += i&1 ? q_len : -q_len;
            children.push_back({n_c, q_len});
        }
    }
    inline bool isLeaf(){ return data == nullptr && children.empty();}
    inline void insert(DataType* d){
        if(isLeaf()){
            data = d;
            return;
        }
        if(children.empty())
            makeChildren();
        if(data != nullptr){
            unsigned char i = getChildIdx(data->pos);
            children[i].insert(data);
            data = nullptr;
        }
        unsigned char i = getChildIdx(d->pos);
        children[i].insert(d);
    }
    inline void print(int depth){
    	if(isLeaf()) return;
		for(int i = 0; i < depth; i++)
			std::cout << " ";
		std::cout << depth << " : " << center.x << " " << center.y << " " << center.z << " : ";
    	if(data){
    		std::cout << data->pos.x << " " << data->pos.y << " " << data->pos.z;
    	}
    	std::cout << std::endl;
    	for(auto& i : children)
    		i.print(depth + 1);
    }
};


struct AABB{
    glm::vec3 bounds[2];
    AABB(OctNode* o){
        bounds[0] = o->center - o->half_len;
        bounds[1] = o->center + o->half_len;
    }
};

struct HitResult{
	Point* pt;
	float t;
	HitResult(Point* p, float _t) : pt(p), t(_t){};
	bool operator < (const HitResult& other){
		return t < other.t;
	}
};


struct Ray{
    glm::vec3 origin, direction, inv_dir;
    unsigned char sign[3];
    Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(d) {
        inv_dir = 1.0f / d;
        sign[0] = inv_dir.x < 0 ? 1 : 0;
        sign[1] = inv_dir.y < 0 ? 1 : 0;
        sign[2] = inv_dir.z < 0 ? 1 : 0;
    }
    inline float intersect(AABB& box){
        glm::vec3 min, max;
        min.x = inv_dir.x * ( box.bounds[sign[0]].x - origin.x );
        max.x = inv_dir.x * ( box.bounds[1 - sign[0]].x - origin.x );
        min.y = inv_dir.y * ( box.bounds[sign[0]].y - origin.y ); 
        max.y = inv_dir.y * ( box.bounds[1 - sign[0]].y - origin.y );
        min.z = inv_dir.z * ( box.bounds[sign[0]].z - origin.z );
        max.z = inv_dir.z * ( box.bounds[1 - sign[0]].z - origin.z );
        min.x = std::max(min.x, std::max(min.y, min.z));
        max.x = std::min(max.x, std::min(max.y, max.z));
        return (min.x > max.x) ? -1.0f : min.x;
    }
    inline void getHitList(OctNode& node, std::vector<HitResult>& list){
    	list.clear();
		std::vector<OctNode*> stack; 
		stack.reserve(512); // 8^3; expect to go at least 3 deep fully
		stack.push_back(&node);
		while(!stack.empty()){
			OctNode* cur = stack.back();
			stack.pop_back();
			AABB box(cur);
			PRINTLINEMACRO
			float t = intersect(box);
			if(t >= 0.0f){
				if(cur->data){
					list.push_back({cur->data, t});
					PRINTLINEMACRO
				}
				else {
					for(OctNode& i : cur->children){
						stack.push_back(&i);
						PRINTLINEMACRO
					}
				}
			}
		}
		PRINTLINEMACRO
		if(list.size() > 8){	// keep only 8 nearest hits
			std::partial_sort(begin(list), begin(list)+8, end(list));
			list.erase(begin(list)+8, end(list));
		}
		else
			std::sort(begin(list), end(list));
    }
};

#endif
