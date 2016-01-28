#ifndef CSGARRAY_H
#define CSGARRAY_H

#include "csg.h"
#include <unordered_map>
#include "mesh.h"

int hash(const glm::vec3& v, float cell_size){
	constexpr int p1 = 999883;
	constexpr int p2 = 984047;
	constexpr int p3 = 964637;
	int x = v.x - fmod(v.x, cell_size);
	int y = v.y - fmod(v.y, cell_size);
	int z = v.z - fmod(v.z, cell_size);
	return x * p1 ^ y * p2 ^ z * p3;
}

struct CSGItem{
	CSGList list;
	VertexBuffer vb;
	bool old;
	CSGItem() : old(true){};
	CSGItem(const CSG& item) : old(true){
		list.push_back(item);
	}
	void insert(const CSG& item){
		list.push_back(item);
		old = true;
	}
	void remesh(float spu){
		if(!old)return;
		old = false;
		vb.clear();
		fillCells(vb, list, spu);
	}
};

struct CSGArray{
	Mesh mesh;
	std::unordered_map<int, CSGItem> data;
	float cell_size;
	bool old;
	CSGArray(float csize) : cell_size(csize), old(true){};
	inline void insert(const CSG& item){
		old = true;
		int key1 = hash(item.min(), cell_size);
		auto i = data.find(key1);
		if(i == std::end(data)){
			data[key1] = CSGItem(item);
		}
		else{
			i->second.insert(item);
		}
		int key2 = hash(item.max(), cell_size);
		if(key1 == key2)return;
		i = data.find(key2);
		if(i == std::end(data)){
			data[key2] = CSGItem(item);
		}
		else{
			i->second.insert(item);
		}
	}
	inline void remesh(float spu){
		if(!old)return;
		old = false;
		VertexBuffer vb;
		for(auto& i : data){
			i.second.remesh(spu);
			vb.insert(std::end(vb), std::begin(i.second.vb), std::end(i.second.vb));
		}
		mesh.upload(vb);
	}
	inline void draw(){
		mesh.draw();
	}
};

#endif
