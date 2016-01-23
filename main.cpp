
#include "myglheaders.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "UBO.h"
#include "mesh.h"
#include "timer.h"

#include "time.h"
#include <random>
#include <iostream>
#include <cfloat>

using namespace std;
using namespace glm;

struct Uniforms{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
};

typedef float (*SDF_Func)(const glm::vec3& c, const glm::vec3& p);

struct CSG{
	vec3 center;
	vec3 params;
	SDF_Func func;
	int material;
	CSG(const vec3& c, const vec3& p, SDF_Func f, int m)
		: center(c), params(p), func(f), material(m){};
	CSG(const CSG& o)
		: center(o.center), params(o.params), func(o.func), material(o.material){};
	CSG& operator = (const CSG& other){
		center = other.center;
		params = other.params;
		func = other.func;
		material = other.material;
		return *this;
	}
	float operator()(const vec3& p){
		return func(p - center, params);
	}
};

typedef std::vector<CSG> CSGList;

float sphere(const glm::vec3& p, const glm::vec3& b){
	return length(p) - b.x;
}

float box(const glm::vec3& p, const glm::vec3& b){
	vec3 d = abs(p) - b;
	return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(glm::max(d, 0.0f));
}

void getBounds(CSGList& list, vec3& lo, vec3& hi){
	lo = vec3(FLT_MAX);
	hi = vec3(FLT_MIN);
	SDF_Func sph = &sphere;
	for(auto& i : list){
		if(i.func == sph){
			lo = glm::min(lo, i.center - i.params.x);
			hi = glm::max(hi, i.center + i.params.x);
		}
		else{
			lo = glm::min(lo, i.center - i.params);
			hi = glm::min(hi, i.center + i.params);
		}
	}
}

float map(const glm::vec3& p, CSGList& list, int& material){
	float min = FLT_MAX;
	int mat = -1;
	for(auto& i : list){
		float f = i.func(p - i.center, i.params);
		if(f < min){
			min = f;
			mat = i.material;
		}
	}
	material = mat;
	return min;
}

float map(const glm::vec3& p, CSGList& list){
	float min = FLT_MAX;
	for(auto& i : list)
		min = glm::min(min, i.func(p - i.center, i.params));
	return min;
}

void fillCells(VertexBuffer& vb, CSGList& list, float spu){
	vec3 dx(0.01f, 0.0f, 0.0f);
	vec3 dy(0.0f, 0.01f, 0.0f);
	vec3 dz(0.0f, 0.0f, 0.01f);
	vec3 min, max;
	getBounds(list, min, max);
	float pitch = 1.0f / spu;
	float psize = 1024.0f * pitch;
	for(float z = min.z; z <= max.z; z += pitch){
		for(float y = min.y; y <= max.y; y += pitch){
			for(float x = min.x; x <= max.x; x += pitch){
				vec3 i(x, y, z);
				if(map(i, list) > pitch)continue;
				for(int j = 0; j < 60; j++){
					float dis = map(i, list);
					vec3 N = normalize(vec3(
						map(i+dx, list) - map(i-dx, list),
						map(i+dy, list) - map(i-dy, list),
						map(i+dz, list) - map(i-dz, list)));
					if(abs(dis) < pitch*0.01f){
						int mat;
						map(i, list, mat);
						vb.push_back(Vertex(i, N, N*0.5f + 0.5f, psize));
						break;
					}
					i -= N * dis;
				}
			}
		}
	}
}

double frameBegin(unsigned& i, double& t){
    double dt = glfwGetTime() - t;
    t += dt;
    i++;
    if(t >= 3.0){
    	double ms = (t / i) * 1000.0;
        printf("ms: %.6f, FPS: %.3f\n", ms, i / t);
        i = 0;
        t = 0.0;
        glfwSetTime(0.0);
    }
    return dt;
}

int main(int argc, char* argv[]){
	if(argc != 3){
		return 1;
	}
	
	srand(time(NULL));
	
	Camera camera;
	const int WIDTH = atoi(argv[1]);
	const int HEIGHT = atoi(argv[2]);
	camera.resize(WIDTH, HEIGHT);
	camera.setEye(vec3(0.0f, 0.0f, 3.0f));
	camera.update();
	
	Window window(WIDTH, HEIGHT, 4, 3, "Meshing");
	Input input(window.getWindow());
	GLProgram colorProg("vert.glsl", "frag.glsl");
    
	Mesh mesh;
	
	CSGList list;
	
	VertexBuffer vb;
	fillCells(vb, list, 25.0f);
	cout << vb.size() << endl;
	mesh.upload(vb);
	vb.clear();
	
	Timer timer;
	
	Uniforms uni;
	uni.light_pos = vec4(5.0f, 5.0f, 5.0f, 0.0f);
	UBO unibuf(&uni, sizeof(uni), 0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
        uni.MVP = camera.getVP();
        uni.eye = vec4(camera.getEye(), 0.0f);
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			uni.light_pos = vec4(camera.getEye(), 0.0f);
		unibuf.upload(&uni, sizeof(uni));
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_Q)){
			list.push_back(CSG(camera.getEye()+camera.getAxis(), vec3(0.5f), &sphere, 1));
			fillCells(vb, list, 25.0f);
			mesh.upload(vb);
			vb.clear();
		}
		
		colorProg.bind();
		timer.begin();
		mesh.draw();
		timer.endPrint();
        window.swap();
    }
    return 0;
}

