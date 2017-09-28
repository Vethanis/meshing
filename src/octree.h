#pragma once

#include "csg.h"
#include "stdlib.h"
#include "circular_queue.h"
#include <mutex>
#include "mesh.h"

namespace oct{

struct OctNode{
    static constexpr u32 LEAF_DEPTH = 6;

    glm::vec3 center;
    float radius;
    u32 children[8];
    u32 leaf_id;
    u32 depth;
    bool hasChildren;

    OctNode(const glm::vec3& c=glm::vec3(0.0f), float _radius=8.0f, u32 d=0)
        : center(c),
        radius(_radius),
        leaf_id(-1),
        depth(d),
        hasChildren(false)
    {}
        
    inline float qlen(){ return 1.732051f * radius; }
    inline bool isLeaf(){return depth == LEAF_DEPTH;}
    inline bool isRoot(){return !depth;}
    inline void makeChildren();
    inline void insert(const CSG& item, u32 csg_id = 0);
    inline void deInit();
};

struct leafData_t{
    CSGIndices items;
    const OctNode* node;
};

struct leafData{
    static constexpr u32 capacity = 1 << (3 * 6);

    leafData_t data[capacity];
    VertexBuffer buffers[capacity];
    Mesh meshes[capacity];

    CircularQueue<u32, 4096> n_remesh, n_update;
    u32 tail;

    leafData() : tail(0){};

    inline void remesh(){
        while(!n_remesh.empty()){
            u32 i = n_remesh.pop();
            {
                leafData_t& item = data[i];
                fillCells(buffers[i], item.items, item.node->center, item.node->radius);
            }
            while(n_update.full()){};
            n_update.push(i);
        }
    }
    // gl thread only
    inline void update(){
        while(!n_update.empty()){
            u32 i = n_update.pop();
            meshes[i].update(buffers[i]);
        }
    }
    // gl thread only
    inline void draw(){
        for(Mesh& i : meshes){
            i.draw();
        }
    }

    inline u32 append(const OctNode* node){
        if(tail >= capacity){
            puts("Ran out of capacity in leafData::append()");
            return tail - 1;
        }
        const u32 idx = tail++;
        data[idx].node = node;
        return idx;
    }

    inline void insert_CSG(u32 leaf, u32 csg){
        data[leaf].items.push_back(csg);
        while(n_remesh.full()){
            remesh();
        }
        n_remesh.set_push(leaf);
    }
};

extern leafData g_leafData;

constexpr u32 max_octnodes = 1 << (3 * 6);
static u32 octnodes_tail = 0;
extern OctNode g_octNode[max_octnodes];
extern std::mutex g_octNode_mut;

void OctNode::makeChildren(){
    std::lock_guard<std::mutex> lock(g_octNode_mut);
    const float nlen = radius * 0.5f;
    for(int i = 0; i < 8; i++){
        glm::vec3 n_c(center);
        n_c.x += (i&4) ? nlen : -nlen;
        n_c.y += (i&2) ? nlen : -nlen;
        n_c.z += (i&1) ? nlen : -nlen;

        children[i] = octnodes_tail;
        ++octnodes_tail;
        OctNode& child = g_octNode[children[i]];
        child = OctNode(n_c, nlen, depth + 1);

        if(child.isLeaf()){
            child.leaf_id = g_leafData.append(this);
        }

        if(octnodes_tail >= max_octnodes){
            puts("Ran out of octnodes in makeChildren");
            return;
        }
    }
    hasChildren = true;
}

void OctNode::insert(const CSG& item, u32 csg_id){
    if(item.func(center) >= qlen() + item.param.smoothness){
        return;
    }
    if(isLeaf()){
        g_leafData.insert_CSG(leaf_id, csg_id);
        return;
    }
    if(isRoot()){
        if(csg_tail >= max_csgs){
            puts("Ran out of room for CSG's in oct::insert();");
            return;
        }
        csg_id = csg_tail;
        ++csg_tail;
        g_CSG[csg_id] = item;
    }
    if(!hasChildren){
        makeChildren();
    }
    for(int i = 0; i < 8; i++){
        g_octNode[children[i]].insert(item, csg_id);
    }
}

extern OctNode g_rootNode;

inline void insert(const CSG& item){
    g_rootNode.insert(item);
}

};