#ifndef OCTREE_H
#define OCTREE_H

#include "csg.h"
#include "stdlib.h"

#define MINLEN 0.125f

struct OctNode{
    OctNode* children;
    float length;
    float qlen;
    glm::vec3 center;
    int depth;
    VertexBuffer vb;
    std::vector<CSG*> items;
    bool old;
    // always instantiate root node with depth 0
    OctNode(const glm::vec3& c, float len=2.0f, int d=0) 
        : children(nullptr), length(len), center(c), depth(d), old(false){
        // ratio of cube's side to diagonal: sqrt(len^2 + len^2 + len^2) => sqrt(len^2 * 3) => x * sqrt(3)
        qlen = 1.732051f * len;
    }
    ~OctNode(){
        if(!depth){
            // root node contains all CSG items
            for(CSG* i : items)
                delete i;
        }
        if(children){
            for(char i = 0; i < 8; i++)
                (children + i)->~OctNode();
            free(children);
        }
    }
    inline bool isLeaf(){return !children;}
    inline void makeChildren(){
        if(children)return;
        const float nlen = length * 0.5f;
        if(nlen < MINLEN)return;
        children = (OctNode*)malloc(sizeof(OctNode) * 8);
        for(char i = 0; i < 8; i++){
            glm::vec3 n_c(center);
            n_c.x += (i&4) ? nlen : -nlen;
            n_c.y += (i&2) ? nlen : -nlen;
            n_c.z += (i&1) ? nlen : -nlen;
            new (children + i) OctNode(n_c, nlen, depth + 1);
        }
        
    }
    // always pass an item on the heap here
    inline void insert(CSG* item){
        if(item->func(center) > qlen){
            if(!depth)
                delete item;
            return;
        }
        items.push_back(item);
        old = true;
        makeChildren();
        if(children){
            for(char i = 0; i < 8; i++)
                children[i].insert(item);
        }
    }
    inline void remesh(VertexBuffer& rootvb, float spu){
        if(isLeaf()){
            if(old){
                vb.clear();
                fillCells(vb, items, center - glm::vec3(length), center + glm::vec3(length), spu);
                old = false;
            }
            rootvb.insert(rootvb.end(), vb.begin(), vb.end());
        }
        else{
            for(char i = 0; i < 8; i++)
                children[i].remesh(rootvb, spu);
        }
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
            else{
                for(char i = 0; i < 8; i++)
                    stack.push_back(children + i);
            }
        }
    }
};


#endif // OCTREE_H
