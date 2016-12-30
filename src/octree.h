#ifndef OCTREE_H
#define OCTREE_H

#include "csg.h"
#include "stdlib.h"
#include <unordered_set>
#include "circular_queue.h"
#include "mesh.h"

namespace oct{

#define LEAF_DEPTH 5

struct OctNode;

struct leafData_t{
    CSGList items;
    VertexBuffer vb;
    glm::vec3 center;
    float length;
};

struct leafData{
    std::vector<leafData_t> data;
    std::vector<Mesh> meshes;
    CircularQueue<size_t, 1024> n_remesh, n_update;
    CSGList all_ops;

    inline void remesh(){
        while(!n_remesh.empty()){
            auto i = n_remesh.pop();
            leafData_t& item = data[i];
            item.vb.clear();
            fillCells(item.vb, item.items, item.center, item.length);
            while(n_update.full()){};
            n_update.push(i);
        }
    }
    // gl thread only
    inline void update(){
        while(!n_update.empty()){
            auto i = n_update.pop();
            meshes[i].update(data[i].vb);
        }
    }
    // gl thread only
    inline void draw(){
        for(auto& i : meshes){
            i.draw();
        }
    }

    inline size_t append(const leafData_t& item){
        data.push_back(item);
        meshes.push_back({});
        return data.size() - 1;
    }

    inline void insert_CSG(size_t i, CSG* item){
        data[i].items.push_back(item);
        while(n_remesh.full()){
            remesh();
        }
        n_remesh.set_push(i);
    }

    inline void capture_CSG(CSG* op){
        all_ops.push_back(op);
    }

    ~leafData(){
        for(auto& i : meshes){
            i.destroy();
        }
        for(CSG* i : all_ops){
            delete i;
        }
    }
};

static leafData LEAF_DATA;

static float octlen = 8.0f;

struct OctNode{
    OctNode* children;
    size_t id;
    glm::vec3 center;
    char depth;
    float length(){
        float len = octlen;
        for(char i = 0; i < depth; i++)
            len *= 0.5f;
        return len;
    }
    float qlen(){
        // ratio of cube's side to diagonal: sqrt(len^2 + len^2 + len^2) => sqrt(len^2 * 3) => x * sqrt(3)
        return 1.732051f * this->length();
    }
    // always instantiate root node with depth 0
    OctNode(const glm::vec3& c, char d=0)
        : children(nullptr), id(0), center(c), depth(d){
        if(depth == LEAF_DEPTH){ // if a leaf
            id = LEAF_DATA.append({{}, {}, center, length()});
        }
    }
    ~OctNode(){
        if(children){
            for(int i = 0; i < 8; i++)
                (children + i)->~OctNode();
            free(children);
        }
    }
    inline bool isLeaf(){return depth == LEAF_DEPTH;}
    inline bool isRoot(){return !depth;}
    inline void makeChildren(){
        if(children)return;
        const float nlen = length() * 0.5f;
        children = (OctNode*)malloc(sizeof(OctNode) * 8);
        for(int i = 0; i < 8; i++){
            glm::vec3 n_c(center);
            n_c.x += (i&4) ? nlen : -nlen;
            n_c.y += (i&2) ? nlen : -nlen;
            n_c.z += (i&1) ? nlen : -nlen;
            new (children + i) OctNode(n_c, depth + 1);
        }
    }
    // always pass an item on the heap here
    inline void insert(CSG* item){
        if(item->func(center) >= qlen()){
            if(isRoot())
                delete item;
            return;
        }
        if(isRoot())
            LEAF_DATA.capture_CSG(item);
        if(isLeaf()){
            LEAF_DATA.insert_CSG(id, item);
            return;
        }
        makeChildren();
        for(int i = 0; i < 8; i++)
            children[i].insert(item);
    }

};


};

#endif // OCTREE_H
