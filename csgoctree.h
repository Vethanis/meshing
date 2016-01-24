#ifndef CSGOCTREE_H
#define CSGOCTREE_H

#include "csg.h"
#include "debugmacro.h"
#include "mesh.h"

struct CSGOctNode{
	glm::vec3 center;
	CSGList list;
	std::vector<CSGOctNode> children;
	Mesh mesh;
	float len;
	CSGOctNode(const glm::vec3& c, float l) : center(c), len(l){};
	~CSGOctNode(){};
	inline unsigned char getChildIdx(const glm::vec3& location){
		unsigned char i = 0;
		i |= (location.x >= center.x) ? 4 : 0;
		i |= (location.y >= center.y) ? 2 : 0;
		i |= (location.z >= center.z) ? 1 : 0;
		return i;
	}
    inline bool hasChildren(){return children.size() == 8;}
    inline void makeChildren(){
    	if(hasChildren()) return;
        children.reserve(8);
        float nlen = len * 0.5f;
        for(unsigned char i=0; i<8; i++){
            glm::vec3 n_c(center);
            n_c.x += (i&4) ? nlen : -nlen;
            n_c.y += (i&2) ? nlen : -nlen;
            n_c.z += (i&1) ? nlen : -nlen;
            children.push_back({n_c, nlen});
        }
    }
};

inline void insert(CSGOctNode* node, const CSG& item){
	std::vector<CSGOctNode*> stack;
	stack.push_back(node);
	while(stack.size()){
		CSGOctNode* cur = stack.back();
		stack.pop_back();
		const unsigned char minidx = cur->getChildIdx(item.min());
		const unsigned char maxidx = cur->getChildIdx(item.max());
		const unsigned char diff = maxidx - minidx;
		if(diff == 7){  // item only fits in current node
			cur->list.push_back(item);
			continue;
		}
		cur->makeChildren();
		if(diff == 0){  // item affects exactly 1 child
			stack.push_back(&(cur->children[minidx]));
			continue;
		}
		// item affects [2, 4] children
		stack.push_back(&(cur->children[minidx]));
		stack.push_back(&(cur->children[maxidx]));
		for(unsigned char i = 1; i < 8; i = i << 1){
			if(0 == (i&diff))continue;  // doesn't span this axis
			const unsigned char idx = minidx | i;
			if(idx == maxidx)continue;  // already inserted
			stack.push_back(&(cur->children[idx]));
		}
	}
}

#endif
