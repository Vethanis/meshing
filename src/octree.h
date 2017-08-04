#pragma once

#include "csg.h"
#include "stdlib.h"
#include "circular_queue.h"
#include <mutex>
#include "mesh.h"

namespace oct{

constexpr int LEAF_DEPTH = 5;

struct leafData_t{
    CSGIndices items;
    glm::vec3 center;
    float length;
};

struct leafData{
    static constexpr u16 capacity = 40000;

    leafData_t data[capacity];
    VertexBuffer buffers[capacity];
    Mesh meshes[capacity];

    CircularQueue<u16, 4096> n_remesh, n_update;
    u16 tail;

    leafData() : tail(0){};

    inline void remesh(){
        while(!n_remesh.empty()){
            u16 i = n_remesh.pop();
            {
                leafData_t& item = data[i];
                fillCells(buffers[i], item.items, item.center, item.length);
            }
            while(n_update.full()){};
            n_update.push(i);
        }
    }
    // gl thread only
    inline void update(){
        while(!n_update.empty()){
            u16 i = n_update.pop();
            meshes[i].update(buffers[i]);
        }
    }
    // gl thread only
    inline void draw(){
        for(Mesh& i : meshes){
            i.draw();
        }
    }

    inline u16 append(const glm::vec3& center, float radius){
        if(tail >= capacity){
            puts("Ran out of capacity in leafData::append()");
            return tail - 1;
        }
        data[tail].center = center;
        data[tail].length = radius;
        return tail++;
    }

    inline void insert_CSG(u16 leaf, u16 csg){
        data[leaf].items.push_back(csg);
        while(n_remesh.full()){
            remesh();
        }
        n_remesh.set_push(leaf);
    }
};

extern leafData g_leafData;

struct OctNode{
    glm::vec3 center;
    float radius;
    u16 children[8];
    u16 leaf_id;
    u16 depth;
    bool hasChildren;
    float qlen(){
        // ratio of cube's side to diagonal: sqrt(len^2 + len^2 + len^2) => sqrt(len^2 * 3) => x * sqrt(3)
        return 1.732051f * radius;
    }
    void deInit();
    OctNode(const glm::vec3& c=glm::vec3(0.0f), float _radius=8.0f, u16 d=0)
        : center(c),
        radius(_radius),
        leaf_id(-1),
        depth(d),
        hasChildren(false)
    {

    }
    inline bool isLeaf(){return depth == LEAF_DEPTH;}
    inline bool isRoot(){return !depth;}
    inline void makeChildren();
    inline void insert(const CSG& item, u16 csg_id = 0);
};

constexpr u16 max_octnodes = 30000;
static u16 octnodes_tail = 0;
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
        OctNode& child = g_octNode[octnodes_tail++];
        child = OctNode(n_c, nlen, depth + 1);

        if(child.isLeaf()){
            child.leaf_id = g_leafData.append(child.center, child.radius);
        }

        if(octnodes_tail >= max_octnodes){
            puts("Ran out of octnodes in makeChildren");
            return;
        }
    }
    hasChildren = true;
}

void OctNode::insert(const CSG& item, u16 csg_id){
    if(item.func(center) >= qlen() + item.param.smoothness){
        return;
    }
    if(isLeaf()){
        g_leafData.insert_CSG(leaf_id, csg_id);
        return;
    }
    if(isRoot()){
        g_CSG[csg_tail] = item;
        csg_id = csg_tail++;    
        if(csg_tail >= max_csgs){
        puts("Ran out of room for CSG's in oct::insert();");
    }
    }
    if(!hasChildren){
        makeChildren();
    }
    for(int i = 0; i < 8; i++){
        u16 index = children[i];
        OctNode& child = g_octNode[index];
        child.insert(item, csg_id);
    }
}

extern OctNode g_rootNode;

inline void insert(const CSG& item){
    g_rootNode.insert(item);
}

};