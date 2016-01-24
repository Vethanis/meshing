#ifndef CSGTREE_H
#define CSGTREE_H

#include "csg.h"
#include "debugmacro.h"
#include "mesh.h"

#define MAX_CSGS 10

enum AXIS{
	NONE,
	X,
	Y,
	Z
};

struct CSGNode{
	glm::vec3 min, max;
	CSGList list;
	CSGNode *left, *right;
	Mesh mesh;
	float center;
	AXIS axis;
	bool old;
	CSGNode() : min(FLT_MAX), max(FLT_MIN), left(nullptr), right(nullptr), center(0.0f), axis(NONE), old(false){}
	~CSGNode(){ delete left; delete right; }
	inline void add(const CSG& item){
		list.push_back(item);
		old = true;
	}
	inline bool full(){return list.size() > MAX_CSGS;}
	inline void remesh(float spu){
		VertexBuffer vb;
		getBounds(list, min, max);
		fillCells(vb, list, min, max, spu);
		mesh.upload(vb);
		old = false;
	}
	inline void draw(){
		mesh.draw();
	}
};

inline void collect(std::vector<CSGNode*>& nodes, CSGNode* node){
	PRINTLINEMACRO
	nodes.push_back(node);
	unsigned i = 0;
	while(true){
		if(nodes[i]->left) nodes.push_back(nodes[i]->left);
		if(nodes[i]->right) nodes.push_back(nodes[i]->right);
		i++;
		if(i >= nodes.size())break;
	}
}

inline void split(CSGNode& node){
	glm::vec3 max, min;
	PRINTLINEMACRO
	getBounds(node.list, min, max);
	PRINTLINEMACRO
	glm::vec3 diff = max - min;
	glm::vec3 center = 0.5f * (min + max);
	float maxv = glm::max(glm::max(diff.x, diff.y), diff.z);
	PRINTLINEMACRO
	if(node.left == nullptr) node.left = new CSGNode;
	if(node.right == nullptr) node.right = new CSGNode;
	PRINTLINEMACRO
	auto* left = node.left;
	auto* right = node.right;
	PRINTLINEMACRO
	if(maxv == diff.x){
		node.center = center.x;
		node.axis = X;
		for(auto i = begin(node.list); i != end(node.list); ){
			PRINTLINEMACRO
			if(i->maxX() < node.center){
				left->add(*i);
				i = node.list.erase(i);
			}
			else if(i->minX() > node.center){
				right->add(*i);
				i = node.list.erase(i);
			}
			else
				++i;
		}
	}
	else if(maxv == diff.y){
		node.center = center.y;
		node.axis = Y;
		for(auto i = begin(node.list); i != end(node.list); ){
			PRINTLINEMACRO
			if(i->maxY() < node.center){
				left->add(*i);
				i = node.list.erase(i);
			}
			else if(i->minY() > node.center){
				right->add(*i);
				i = node.list.erase(i);
			}
			else
				++i;
		}
	}
	else{
		node.center = center.z;
		node.axis = Z;
		for(auto i = begin(node.list); i != end(node.list); ){
			PRINTLINEMACRO
			if(i->maxZ() < node.center){
				left->add(*i);
				i = node.list.erase(i);
			}
			else if(i->minZ() > node.center){
				right->add(*i);
				i = node.list.erase(i);
			}
			else
				++i;
		}
	}
}

inline void insert(CSGNode* node, const CSG& item){
	CSGNode* cur = node;
	PRINTLINEMACRO
	while(cur){
		switch(cur->axis){
			case NONE:{
				PRINTLINEMACRO
				cur->add(item);
				if(cur->full()) split(*cur);
				return;
			}break;
			case X:{
				PRINTLINEMACRO
				if(item.maxX() < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else if(item.minX() > cur->center){
					if(cur->right) cur = cur->right;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else{
					cur->add(item);
					return;
				}
			}break;
			case Y:{
				PRINTLINEMACRO		
				if(item.maxY() < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else if(item.minY() > cur->center){
					if(cur->right) cur = cur->right;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else{
					cur->add(item);
					return;
				}
			}break;
			case Z:{
				PRINTLINEMACRO
				if(item.maxZ() < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else if(item.minZ() > cur->center){
					if(cur->right) cur = cur->right;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else{
					cur->add(item);
					return;
				}
			}break;
		}
	}
}

#endif
