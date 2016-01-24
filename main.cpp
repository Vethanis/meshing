
#include "myglheaders.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "UBO.h"
#include "mesh.h"
#include "timer.h"
#include "csgtree.h"


using namespace std;
using namespace glm;

struct Uniforms{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
};

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
	
	CSGNode csgroot;
	
	VertexBuffer vb;
	
	Timer timer;
	
	Uniforms uni;
	uni.light_pos = vec4(5.0f, 5.0f, 5.0f, 0.0f);
	UBO unibuf(&uni, sizeof(uni), 0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    int waitcounter = 10;
    while(window.open()){
    	waitcounter--;
        input.poll(frameBegin(i, t), camera);
        uni.MVP = camera.getVP();
        uni.eye = vec4(camera.getEye(), 0.0f);
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			uni.light_pos = vec4(camera.getEye(), 0.0f);
		unibuf.upload(&uni, sizeof(uni));
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_Q) && waitcounter < 0){
			waitcounter = 30;
			insert(&csgroot, CSG(camera.getEye()+3.0f*camera.getAxis(), vec3(0.333333f), &sphere, 1));
			std::vector<CSGList*> lists;
			getLists(lists, csgroot);
			for(CSGList* lp : lists)
				fillCells(vb, *lp, 15.0f);
			mesh.upload(vb);
			vb.clear();
		}
		
		colorProg.bind();
		//timer.begin();
		mesh.draw();
		//timer.endPrint();
        window.swap();
    }
    return 0;
}

