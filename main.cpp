
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
	return glm::max(sphere(p, 0.7f), box(p, vec3(0.5f)));
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
	vector<vec3> points;
	vec3 dx(0.1f, 0.0f, 0.0f);
	vec3 dy(0.0f, 0.1f, 0.0f);
	vec3 dz(0.0f, 0.0f, 0.1f);
	for(int z = -100; z < 100; z++){
		float fz = z * 0.05f;
		for(int y = -100; y < 100; y++){
			float fy = y * 0.05f;
			for(int x = -100; x < 100; x++){
				float fx = x * 0.05f;
				vec3 fv(fx, fy, fz);
				if(map(fv) > 0.1f)continue;
				points.push_back(fv);
			}
		}
	}
	for(auto& i : points){
		for(int j = 0; j < 300; j++){
			float dis = map(i);
			vec3 N = vec3(
				map(i+dx) - map(i-dx),
				map(i+dy) - map(i-dy),
				map(i+dz) - map(i-dz));
			if(abs(dis) < 0.01f){
				vb.push_back(Vertex(i, normalize(N), vec3(1.0f), 50.0f));
				break;
			}
			i -= normalize(N) * dis;
		}
	}
	points.clear();
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

