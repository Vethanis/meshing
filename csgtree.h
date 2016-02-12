#ifndef CSGTREE_H
#define CSGTREE_H

#include "csg.h"
#include "debugmacro.h"
#include "mesh.h"

#define MAX_CSGS 20

enum AXIS{
	NONE,
	X,
	Y,
	Z
};

struct CSGNode{
	CSGList list;
	CSGNode *left, *right;
	VertexBuffer vb;
	float center;
	AXIS axis;
	bool old;
	CSGNode() : left(nullptr), right(nullptr), center(0.0f), axis(NONE), old(true){}
	~CSGNode(){ delete left; delete right; }
	inline void add(const CSG& item){
		list.push_back(item);
		old = true;
	}
	inline bool full(){return list.size() > MAX_CSGS;}
	inline void remesh(VertexBuffer& rootvb, float spu){
		if(old){
			vb.clear();
			fillCells(vb, list, spu);
			old = false;
		}
		rootvb.insert(rootvb.end(), vb.begin(), vb.end());
	}
};

inline void collect(std::vector<CSGNode*>& nodes, CSGNode* node){
	nodes.push_back(node);
	unsigned i = 0;
	while(true){
		if(nodes[i]->left){
			nodes.push_back(nodes[i]->left);
			nodes.push_back(nodes[i]->right);
		}
		i++;
		if(i >= nodes.size())break;
	}
}

inline void split(CSGNode& node){
	glm::vec3 max, min;
	getBounds(node.list, min, max);
	glm::vec3 diff = glm::abs(max - min);
	glm::vec3 center = 0.5f * (min + max);
	float maxv = glm::max(glm::max(diff.x, diff.y), diff.z);
	if(node.left == nullptr){
		node.left = new CSGNode;
		node.right = new CSGNode;
	}
	auto* left = node.left;
	auto* right = node.right;
	if(maxv == diff.x){
		node.center = center.x;
		node.axis = X;
		for(auto i = begin(node.list); i != end(node.list); ){
			if(i->max().x < node.center){
				left->add(*i);
				i = node.list.erase(i);
			}
			else if(i->min().x > node.center){
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
			if(i->max().y < node.center){
				left->add(*i);
				i = node.list.erase(i);
			}
			else if(i->min().y > node.center){
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
			if(i->max().z < node.center){
				left->add(*i);
				i = node.list.erase(i);
			}
			else if(i->min().z > node.center){
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
	while(cur){
		switch(cur->axis){
			case NONE:{
				cur->add(item);
				if(cur->full()) split(*cur);
				return;
			}break;
			case X:{
				if(item.max().x < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else if(item.min().x > cur->center){
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
				if(item.max().y < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else if(item.min().y > cur->center){
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
				if(item.max().z < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->add(item);
						if(cur->full()) split(*cur);
						return;
					}
				}
				else if(item.min().z > cur->center){
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
