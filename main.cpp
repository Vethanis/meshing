
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

using namespace std;
using namespace glm;

struct Uniforms{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
};

float sphere(const glm::vec3& p, float r){
	return length(p) - r;
}

float box(const glm::vec3& p, const glm::vec3& b){
	vec3 d = abs(p) - b;
	return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + length(glm::max(d, 0.0f));
}

float map(const glm::vec3& p){
	//return sphere(p, 0.5f);
	//return box(p, vec3(1.0f));
	vector<float> parts;
	parts.push_back(sphere(p, 0.7f)); // body
	parts.push_back(sphere(p-vec3(0.7f, 0.1f, 0.0f), 0.4f)); // head
	parts.push_back(sphere(p-vec3(1.0f, 0.0f, 0.0f), 0.2f)); // snout
	parts.push_back(sphere(p-vec3(1.1f, 0.2f, 0.1f), 0.05f)); // near eye
	parts.push_back(sphere(p-vec3(1.1f, 0.2f, -0.1f), 0.05f)); // far eye
	parts.push_back(sphere(p-vec3(0.9f, 0.4f, 0.2f), 0.1f)); // near ear
	parts.push_back(sphere(p-vec3(0.9f, 0.4f, -0.2f), 0.1f)); // far ear
	parts.push_back(sphere(p-vec3(0.4f, -0.4f, 0.4f), 0.2f));	// legs
	parts.push_back(sphere(p-vec3(0.4f, -0.4f, -0.4f), 0.2f));	// legs
	parts.push_back(sphere(p-vec3(-0.4f, -0.4f, 0.4f), 0.2f));	// legs
	parts.push_back(sphere(p-vec3(-0.4f, -0.4f, -0.4f), 0.2f));	// legs
	parts.push_back(sphere(p-vec3(-0.7f, 0.1f, 0.0f), 0.1f));	// tail
	float min = parts[0];
	for(auto i : parts)
		min = (i < min) ? i : min;
	return min;
}

void fillCells(VertexBuffer& vb, const glm::vec3& min, const glm::vec3& max, float dim){
	vec3 dx(0.01f, 0.0f, 0.0f);
	vec3 dy(0.0f, 0.01f, 0.0f);
	vec3 dz(0.0f, 0.0f, 0.01f);
	vec3 ddim = max - min;
	vec3 pitch = ddim / dim;
	float pavg = (pitch.x+pitch.y+pitch.z)*0.3333333f;
	float psize = pavg * 1024.0f;
	float threshold = pavg;
	for(float z = min.z; z <= max.z; z += pitch.z){
		for(float y = min.y; y <= max.y; y += pitch.y){
			for(float x = min.x; x <= max.x; x += pitch.x){
				vec3 i(x, y, z);
				if(map(i) > threshold)continue;
				for(int j = 0; j < 60; j++){
					float dis = map(i);
					vec3 N = normalize(vec3(
						map(i+dx) - map(i-dx),
						map(i+dy) - map(i-dy),
						map(i+dz) - map(i-dz)));
					if(abs(dis) < threshold*0.001f){
						vb.push_back(Vertex(i, N, vec3(1.0f, 0.5f, 0.5f), psize));
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
	
	VertexBuffer vb;
	fillCells(vb, vec3(-2.0f), vec3(2.0f), 100.0f);
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
		
		colorProg.bind();
		timer.begin();
		mesh.draw();
		timer.endPrint();
        window.swap();
    }
    return 0;
}

