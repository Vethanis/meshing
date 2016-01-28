
#include "csg.h"
#include <set>
#include <pair>

class CSGCell{
	VertexBuffer vb;
	std::vector< CSGList > data;
	glm::vec3 min;
	float dim;
	float cell_size;
	int depth;
	int width;
	int items;
	bool old;
public:
	CSGCell(const glm::vec3& m, float dimension, int d) : min(m), dim(dimension), depth(d), items(0), old(true){
		width = 1 << depth;
		cell_size = dim / width;
		for(int i = 0; i < width*width*width; i++)
			data.push_back(CSGList);
	}
	inline CSGList* get(const glm::vec3& p){
		glm::ivec3 iv((p - min) / cell_size);
		long i = iv.x + iv.y*width + iv.z*width*width;
		return &data[i];
	}
	inline glm::vec3 min()const{ return min;}
	inline glm::vec3 max()const{ return min + dim;}
	inline int items()const{return items;}
	inline bool old()const{return old;}
	inline bool overfilled()const{return items > 10 * width*width*width;}
	inline std::vector< CSGList >& getData(){return data;}
	inline VertexBuffer& getBuffer(){return vb;}
	inline void insert(const CSG& i){
		glm::vec3 imin = i.min();
		glm::vec3 imax = i.max();
		imin = glm::clamp(imin, min, min+dim);
		imax = glm::clamp(imax, min, min+dim);
		std::set<CSGList*> cell_set;
		for(float z = imin.z; z < imax.z; z+= cell_size;){
		for(float y = imin.y; y < imax.y; y+= cell_size;){
		for(float x = imin.x; x < imax.x; x+= cell_size;){
			cell_set.insert(get(glm::vec3(x, y, z)));
		}}}
		for(CSGList* j : cell_set)
			j->push_front(i);
		items++;
		old = true;
	}
	inline void resize(int depth){
		std::set<CSG> temp;
		for(CSGList& i : data){
			for(CSG& j : i)
				temp.insert(j);
		}
		data.clear();
		width = 1 << depth;
		cell_size = dim / width;
		for(int i = 0; i < width*width*width; i++)
			data.push_back(CSGList);
		for(CSG& i : temp)
			this->insert(i);
		old = true;
	}
	inline void remesh(float spu){		
		if(!old)return;
		vb.clear();
		if(overfilled())resize(depth+1);
		for(CSGList& i : data)
			fillCells(vb, i, spu);
		old = false;
	}
};

class CSGChunk{
	Mesh mesh;
	std::vector<CSGCell> data;	
	glm::vec3 min;
	float dim;
	float cell_size;
	int width;
	bool old;
public:
	CSGChunk(const glm::vec3& m, float dimension, int depth) : min(m), dim(dimension), old(true){
		width = 1 << depth;
		cell_size = dim / width;
		for(int n = 0; i < width; n++){
		for(int r = 0; y < width; r++){
		for(int c = 0; x < width; c++){
			glm::vec3 cmin(min + glm::vec3(c, r, n) * cell_size);
			data.push_back(CSGCell(cmin, cell_size, depth+1));
		}}}
	}
	inline CSGCell* get(const glm::vec3& p){
		glm::ivec3 iv((p - min) / cell_size);
		long i = iv.x + iv.y*width + iv.z*width*width;
		return &data[i];
	}
	inline glm::vec3 min()const{ return min;}
	inline glm::vec3 max()const{ return min + dim;}
	inline bool old()const{return old;}
	inline std::vector< CSGCell >& getData(){return data;}
	inline void insert(const CSG& i){
		glm::vec3 imin = i.min();
		glm::vec3 imax = i.max();
		imin = glm::clamp(imin, min, min+dim);
		imax = glm::clamp(imax, min, min+dim);
		std::set<CSGCell*> cell_set;
		for(float z = imin.z; z < imax.z; z+= cell_size;){
		for(float y = imin.y; y < imax.y; y+= cell_size;){
		for(float x = imin.x; x < imax.x; x+= cell_size;){
			cell_set.insert(get(glm::vec3(x, y, z)));
		}}}
		for(CSGCell* j : cell_set)
			j->insert(i);
		old = true;
	}
	inline void draw(){mesh.draw();}
	inline void remesh(float spu){
		if(!old)return;
		VertexBuffer vb;
		for(CSGCell& i : data){
			i.remesh(spu);
			vb.insert(std::end(vb), std::begin(i.getBuffer()), std::end(i.getBuffer));
		}
		mesh.upload(vb);
	}
};

#define CSGWORLDITERATE(x){	\
	for(int n = 0; n < 3; n++){	\
	for(int r = 0; r < 3; r++){	\
	for(int c = 0; c < 3; c++){	\
		x	\
	}}}	\
}

class CSGWorld{
	CSGChunk* data[3][3][3];
	glm::vec3 min;
	float dim;
	int depth;
	float cell_size;
	bool old;
	CSGWorld(const glm::vec3& m, float dimension, int d) : min(m), dim(dimension), depth(d), old(true){
		cell_size = dim / 3;
		CSGWORLDITERATE(
			glm::vec3 cmin(min + glm::vec3(c, r, n) * cell_size);
			data[n][r][c] = new CSGChunk(cmin, cell_size, depth);
		)
	}
	~CSGWorld(){
		CSGWORLDITERATE(delete data[n][r][c];)
	}
	inline bool contains(const glm::vec3& pos){
		if(pos.x < min.x || pos.x > min.x + cell_size*3.0f)return false;
		if(pos.y < min.y || pos.y > min.y + cell_size*3.0f)return false;
		if(pos.z < min.z || pos.z > min.z + cell_size*3.0f)return false;
		return true;
	}
	inline CSGChunk* get(const glm::vec3& p){
		glm::ivec3 iv((p - min) / cell_size);
		return data[iv.z][iv.y][iv.x];
	}
	inline void insert(const CSG& i){
		glm::vec3 imin = i.min();
		glm::vec3 imax = i.max();
		imin = glm::clamp(imin, min, min+dim - 0.1f);
		imax = glm::clamp(imax, min, min+dim - 0.1f);
		std::set<CSGCell*> cell_set;
		for(float z = imin.z; z < imax.z; z+= cell_size;){
		for(float y = imin.y; y < imax.y; y+= cell_size;){
		for(float x = imin.x; x < imax.x; x+= cell_size;){
			cell_set.insert(get(glm::vec3(x, y, z)));
		}}}
		for(CSGCell* j : cell_set)
			j->insert(i);
		old = true;
	}
	inline void remesh(float spu){
		if(!old)return;
		old = false;
		CSGWORLDITERATE((data[n][r][c])->remesh(spu);)
	}
	inline void draw(){
		CSGWORLDITERATE((data[n][r][c])->draw();)
	}
	inline int getID(const glm::vec3& a_min){
		int p1 = 73856093;
		int p2 = 19349663;
		int p3 = 83492791;
		int x = a_min.x / cell_size;
		int y = a_min.y / cell_size;
		int z = a_min.z / cell_size;
		return x * p1 ^ y * p2 ^ z * p3;
	}
	inline void move(const glm::vec3& pos){
		int x = ((pos.x - min.x) / cell_size) - 1.0f; // delta index of center
		int y = ((pos.y - min.y) / cell_size) - 1.0f;
		int z = ((pos.z - min.z) / cell_size) - 1.0f;
		if(!(x | y | z))return; // if no difference, return
		min += glm::vec3((float)x, (float)y, (float)z) * cell_size; // relocate min
		std::vector< std::pair<int, CSGChunk*> > idmap;
		CSGWORLDITERATE(idmap.push_back({getID(data[n][r][c]->getMin()), data[n][r][c]});)
		CSGWORLDITERATE(
			glm::vec3 id_min = min + glm::vec3(c*cell_size, r*cell_size, n*cell_size);
			int id = getID(id_min);
			auto it = std::find(std::begin(idmap), std::end(idmap), id);
			if(it != std::end(idmap)){
				data[n][r][c] = it->second;
				idmap.erase(it);
				continue;
			}
			data[n][r][c] = new CSGChunk(id_min, cell_size, depth);	// load from redis in future
		)
		for(auto i : idmap)
			delete i->second;	// store in redis in future
	}

};
