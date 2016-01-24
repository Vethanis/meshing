#ifndef CSGTREE_H
#define CSGTREE_H

#include "csg.h"
#include "debugmacro.h"

#define MAX_CSGS 10

enum AXIS{
	NONE,
	X,
	Y,
	Z
};

struct CSGNode{
	CSGList list;
	CSGNode *left, *right;
	float center;
	AXIS axis;
	CSGNode() : left(nullptr), right(nullptr), center(0.0f), axis(NONE){}
	~CSGNode(){ delete left; delete right; }
};

inline void getLists(std::vector<CSGList*>& lists, CSGNode& node){
	PRINTLINEMACRO
	std::vector<CSGNode*> nodes;
	nodes.push_back(&node);
	while(nodes.size()){
		CSGNode* cur = nodes.back();
		nodes.pop_back();
		if(cur->left) nodes.push_back(cur->left);
		if(cur->right) nodes.push_back(cur->right);
		lists.push_back(&(cur->list));
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
	auto& llist = node.left->list;
	auto& rlist = node.right->list;
	PRINTLINEMACRO
	if(maxv == diff.x){
		node.center = center.x;
		node.axis = X;
		for(auto i = begin(node.list); i != end(node.list); ){
			PRINTLINEMACRO
			if(i->maxX() < node.center){
				llist.push_back(*i);
				i = node.list.erase(i);
			}
			else if(i->minX() > node.center){
				rlist.push_back(*i);
				i = node.list.erase(i);
			}
			++i;
		}
	}
	else if(maxv == diff.y){
		node.center = center.y;
		node.axis = Y;
		for(auto i = begin(node.list); i != end(node.list); ){
			PRINTLINEMACRO
			if(i->maxY() < node.center){
				llist.push_back(*i);
				i = node.list.erase(i);
			}
			else if(i->minY() > node.center){
				rlist.push_back(*i);
				i = node.list.erase(i);
			}
			++i;
		}
	}
	else{
		node.center = center.z;
		node.axis = Z;
		for(auto i = begin(node.list); i != end(node.list); ){
			PRINTLINEMACRO
			if(i->maxZ() < node.center){
				llist.push_back(*i);
				i = node.list.erase(i);
			}
			else if(i->minZ() > node.center){
				rlist.push_back(*i);
				i = node.list.erase(i);
			}
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
				cur->list.push_back(item);
				if(cur->list.size() > MAX_CSGS) split(*cur);
				return;
			}break;
			case X:{
				PRINTLINEMACRO
				if(item.maxX() < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->list.push_back(item);
						if(cur->list.size() > MAX_CSGS) split(*cur);
						return;
					}
				}
				else if(item.minX() > cur->center){
					if(cur->right) cur = cur->right;
					else{
						cur->list.push_back(item);
						if(cur->list.size() > MAX_CSGS) split(*cur);
						return;
					}
				}
				else{
					cur->list.push_back(item);
					return;
				}
			}break;
			case Y:{
				PRINTLINEMACRO		
				if(item.maxY() < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->list.push_back(item);
						if(cur->list.size() > MAX_CSGS) split(*cur);
						return;
					}
				}
				else if(item.minY() > cur->center){
					if(cur->right) cur = cur->right;
					else{
						cur->list.push_back(item);
						if(cur->list.size() > MAX_CSGS) split(*cur);
						return;
					}
				}
				else{
					cur->list.push_back(item);
					return;
				}
			}break;
			case Z:{
				PRINTLINEMACRO
				if(item.maxZ() < cur->center){
					if(cur->left) cur = cur->left;
					else{
						cur->list.push_back(item);
						if(cur->list.size() > MAX_CSGS) split(*cur);
						return;
					}
				}
				else if(item.minZ() > cur->center){
					if(cur->right) cur = cur->right;
					else{
						cur->list.push_back(item);
						if(cur->list.size() > MAX_CSGS) split(*cur);
						return;
					}
				}
				else{
					cur->list.push_back(item);
					return;
				}
			}break;
		}
	}
}

#endif
