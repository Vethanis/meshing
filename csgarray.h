#ifndef CSGARRAY_H
#define CSGARRAY_H

#include "csg.h"
#include <unordered_map>
#include <set>
#include <algorithm>
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
	std::unordered_map<int, CSGItem> data;
	float cell_size;
	bool old;
	CSGArray(float csize) : cell_size(csize), old(true){};
	inline void getPoints(std::vector<glm::vec3>& pts, const CSG& item){
		glm::vec3 lo = item.min();
		glm::vec3 hi = item.max();
		pts.push_back(lo);
		pts.push_back({lo.x, lo.y, hi.z});
		pts.push_back({lo.x, hi.y, lo.z});
		pts.push_back({lo.x, hi.y, hi.z});
		pts.push_back({hi.x, lo.y, lo.z});
		pts.push_back({hi.x, lo.y, hi.z});
		pts.push_back({hi.x, hi.y, lo.z});
		pts.push_back(hi);
	}
	inline void insert(const CSG& item){
		old = true;
		std::vector<glm::vec3> points;
		getPoints(points, item);
		std::set<int> keys;
		for(auto& i : points){
			keys.insert(hash(i, cell_size));
		}
		for(auto j : keys){
			auto i = data.find(j);
			if(i == std::end(data)){
				data[j] = CSGItem(item);
			}
			else{
				i->second.insert(item);
			}
		}
	}
	inline void remesh(VertexBuffer& vb, float spu){
		if(!old)return;
		old = false;
		vb.clear();
		for(auto& i : data){
			i.second.remesh(spu);
			vb.insert(std::end(vb), std::begin(i.second.vb), std::end(i.second.vb));
		}
	}
};

#endif
