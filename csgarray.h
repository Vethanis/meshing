#ifndef CSGARRAY_H
#define CSGARRAY_H

#include "csg.h"
#include <unordered_map>
#include <set>
#include <algorithm>
#include "mesh.h"
#include "csgtree.h"

unsigned hash(const glm::vec3& v, float cell_size){
	constexpr unsigned p1 = 2147483647;
	constexpr unsigned p2 = 981986576;
	constexpr unsigned p3 = 1359374631;
	unsigned x = v.x - fmod(v.x, cell_size);
	unsigned y = v.y - fmod(v.y, cell_size);
	unsigned z = v.z - fmod(v.z, cell_size);
	return (x * p1) ^ (y * p2) ^ (z * p3);
}

struct CSGItem{
	CSGNode root;
	Mesh mesh;
	VertexBuffer vb;
	unsigned items;
	bool old, initialised;
	CSGItem() : items(0), old(true), initialised(false){};
	inline void init(){
		mesh.init();
		initialised = true;
		printf("Mesh created with id: %u\n", mesh.vbo);
	}
	inline void insert(const CSG& item){
		::insert(&root, item);
		old = true;
		items++;
	}
	inline void remesh(float spu){
		if(!old)return;
		vb.clear();
		std::vector<CSGNode*> nodes;
		nodes.reserve(glm::max(2, (int)items / MAX_CSGS));
		collect(nodes, &root);
		for(CSGNode* i : nodes){
			i->remesh(vb, spu);
		}
	}
	inline void update(){
		if(!old)return;
		if(!initialised)this->init();
		mesh.update(vb);
		vb.clear();
		old = false;
	}
	inline void draw(){
		mesh.draw();
	}
	inline void destroy(){
		mesh.destroy();
	}
};

struct CSGArray{
	std::unordered_map<unsigned, CSGItem> data;
	float cell_size;
	CSGArray(float csize) : cell_size(csize){
	};
	~CSGArray(){
		for(auto& i : data){
			i.second.destroy();
		}
	}
	inline void getPoints(std::vector<glm::vec3>& pts, const CSG& item){
		glm::vec3 lo = item.min();
		glm::vec3 hi = item.max();
		glm::vec3 pitch = glm::min(hi-lo, glm::vec3(cell_size));
		for(float z = lo.z; z <= hi.z; z += pitch.z){
		for(float y = lo.y; y <= hi.y; y += pitch.y){
		for(float x = lo.x; x <= hi.x; x += pitch.x){
			pts.push_back({x, y, z});
		}}}
	}
	inline void insert(const CSG& item){
		std::vector<glm::vec3> points;
		getPoints(points, item);
		std::set<unsigned> keys;
		for(auto& i : points){
			keys.insert(hash(i, cell_size));
		}
		for(unsigned k : keys){
			auto itb = data.insert({k, CSGItem()});
			itb.first->second.insert(item);
		}
	}
	inline void remesh(float spu){
		for(auto& i : data){
			i.second.remesh(spu);
		}
	}
	inline void update(){
		for(auto& i : data){
			i.second.update();
		}
	}
	inline void draw(){
		for(auto& i : data){
			i.second.draw();
		}
	}
};

#endif
