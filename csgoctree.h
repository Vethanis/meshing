#ifndef CSGOCTREE_H
#define CSGOCTREE_H

#include "csg.h"
#include "debugmacro.h"
#include "mesh.h"
#include <stack>

// instantiate root node with depth of 1

struct CSGOctNode{
	glm::vec3 center;
	CSGList list;
	std::vector<CSGOctNode> children;
	VertexBuffer vb;
	float len;
	int depth;
	bool old;
	CSGOctNode(const glm::vec3& c, float l, int d) : center(c), len(l), depth(d), old(true){PRINTLINEMACRO};
	~CSGOctNode(){};
	inline CSGOctNode* getChild(unsigned char i){
		return &(children[i]);
	}
	inline bool hasData(){ return list.size() > 0;}
	inline unsigned char getChildIdx(const glm::vec3& location){
		PRINTLINEMACRO
		unsigned char i = 0;
		i |= (location.x >= center.x) ? 4 : 0;
		i |= (location.y >= center.y) ? 2 : 0;
		i |= (location.z >= center.z) ? 1 : 0;
		return i;
	}
    inline bool hasChildren(){return children.size() == 8;}
    inline void makeChildren(){
    	PRINTLINEMACRO
    	if(hasChildren()) return;
        children.reserve(8);
        float nlen = len * 0.5f;
        for(unsigned char i=0; i<8; i++){
            glm::vec3 n_c(center);
            n_c.x += (i&4) ? nlen : -nlen;
            n_c.y += (i&2) ? nlen : -nlen;
            n_c.z += (i&1) ? nlen : -nlen;
            children.push_back({n_c, nlen, depth+1});
        }
    }
    inline void addItem(const CSG& item){
    	list.push_back(item);
    	old = true;
    }
    inline void remesh(float spu){
    	PRINTLINEMACRO
    	if(old){
			fillCells(vb, list, center - len, center + len, spu);
		}
		old = false;
	}
};

inline void collect(std::vector<CSGOctNode*>& out, CSGOctNode* root){
	PRINTLINEMACRO
	out.clear();
	out.reserve(8);
	std::stack<CSGOctNode*> stack;
	stack.push(root);
	while(stack.size()){
		PRINTLINEMACRO
		CSGOctNode* cur = stack.top();
		stack.pop();
		PRINTLINEMACRO
		if(cur->old || cur->hasData()){
			PRINTLINEMACRO
			out.push_back(cur);
		}
		PRINTLINEMACRO
		if(cur->hasChildren()){
			PRINTLINEMACRO
			for(int i = 0; i < 8; i++){
				PRINTLINEMACRO
				stack.push(cur->getChild(i));
			}
		}
	}
}

inline void insert(CSGOctNode* node, const CSG& item){
	PRINTLINEMACRO
	std::stack<CSGOctNode*> stack;
	stack.push(node);
	while(stack.size()){
		CSGOctNode* cur = stack.top();
		stack.pop();
		if(cur->len < 0.5f){
			cur->addItem(item);
			continue;
		}
		const unsigned char minidx = cur->getChildIdx(item.min());
		const unsigned char maxidx = cur->getChildIdx(item.max());
		const unsigned char diff = maxidx - minidx;
		if(diff == 7){  // item only fits in current node
			cur->addItem(item);
			continue;
		}
		cur->makeChildren();
		if(diff == 0){  // item affects exactly 1 child
			stack.push(cur->getChild(minidx));
			continue;
		}
		// item affects [2, 4] children
		stack.push(cur->getChild(minidx));
		stack.push(cur->getChild(maxidx));
		for(unsigned char i = 1; i < 8; i = i << 1){
			if(0 == (i&diff))continue;  // doesn't span this axis
			const unsigned char idx = minidx | i;
			if(idx == maxidx)continue;  // already inserted
			stack.push(cur->getChild(idx));
		}
	}
}

#endif
