#ifndef OCTREE_H
#define OCTREE_H

#include "csg.h"

#define MINLEN 0.1f

struct OctNode{
	glm::vec3 center;
	float length;
	float qlen;
	int depth;
	bool old;
	VertexBuffer vb;
	std::vector<CSG*> items;
	std::vector<OctNode> children;
	// always instantiate root node with depth 0
	OctNode(const glm::vec3& c, float len=2.0f, int d=0) : center(c), length(len), depth(d), old(false){
		children.clear();
		qlen = glm::length(glm::vec3(length));
	}
	~OctNode(){
		if(depth == 0){
			// root node contains all CSG items
			for(CSG* i : items)
				delete i;
		}
	}
	inline void makeChildren(){
		if(children.size())return;
		const float nlen = length * 0.5f;
		if(nlen < MINLEN)return;
		children.reserve(8);
		const int ndepth = depth + 1;
		for(int i = 0; i < 8; i++){
            glm::vec3 n_c(center);
            n_c.x += (i&4) ? nlen : -nlen;
            n_c.y += (i&2) ? nlen : -nlen;
            n_c.z += (i&1) ? nlen : -nlen;
			children.push_back(OctNode(n_c, nlen, ndepth));
		}
	}
	// always pass an item on the heap here
	inline void insert(CSG* item){
		if(item->func(center) > qlen)return;
		items.push_back(item);
		old = true;
		makeChildren();
		for(auto& i : children)
			i.insert(item);
	}
	inline bool isLeaf(){return children.size() == 0;}
	inline void remesh(VertexBuffer& rootvb, float spu){
		if(isLeaf()){
			if(old){
				vb.clear();
				fillCells(vb, items, center - glm::vec3(length), center + glm::vec3(length), spu);
				old = false;
			}
			rootvb.insert(rootvb.end(), vb.begin(), vb.end());
		}
		for(OctNode& i : children)
			i.remesh(rootvb, spu);
	}
	inline void collectLeaves(std::vector<OctNode*>& list){
		list.clear();
		std::vector<OctNode*> stack;
		stack.push_back(this);
		while(stack.size()){
			OctNode* cur = stack.back();
			stack.pop_back();
			if(cur->isLeaf())
				list.push_back(cur);
			for(OctNode& i : cur->children)
				stack.push_back(&i);
		}
	}
};


#endif // OCTREE_H
