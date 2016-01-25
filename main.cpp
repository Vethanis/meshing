
#include "myglheaders.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "UBO.h"
#include "mesh.h"
#include "timer.h"
#include "csgoctree.h"


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
	
	CSGOctNode csgroot(vec3(0.0f), 20.0f, 1);
	Mesh brushMesh;
	Mesh mesh;
	VertexBuffer vb;
	
	Timer timer;
	
	Uniforms uni;
	uni.light_pos = vec4(5.0f, 5.0f, 5.0f, 0.0f);
	UBO unibuf(&uni, sizeof(uni), 0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	SDF_Base* brush = &SPHERESADD;
	float bsize = 0.2f;
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    int waitcounter = 10;
    bool brush_changed = true;
    bool edit = false;
    float spu = 1.0f;
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
    	waitcounter--;
    	
    	glm::vec3 at = camera.getEye() + 3.0f * camera.getAxis();
        
        mat4 brushMat = glm::translate(glm::mat4(), at);
        mat4 VP = camera.getVP();
        uni.MVP = VP * brushMat;
        uni.eye = vec4(camera.getEye(), 0.0f);
        
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			uni.light_pos = vec4(camera.getEye(), 0.0f);
			
		uni.light_pos.w = 0.0f;
		unibuf.upload(&uni, sizeof(uni));
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_UP)){ bsize *= 1.1f; brush_changed = true;}
		else if(glfwGetKey(window.getWindow(), GLFW_KEY_DOWN)){ bsize *= 0.9f; brush_changed = true;}
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_1)){ brush = &SPHERESADD; brush_changed = true;}
		else if(glfwGetKey(window.getWindow(), GLFW_KEY_2)){ brush = &BOXSADD; brush_changed = true;}
		if(glfwGetKey(window.getWindow(), GLFW_KEY_3)){ spu *= 1.01f;}
		else if(glfwGetKey(window.getWindow(), GLFW_KEY_4)){spu *= 0.99f;}
		
		if(brush_changed){
			VertexBuffer brushbuf;
			fillCells(brushbuf, CSG(vec3(0.0f), vec3(bsize), brush, bsize, 1), spu*10.0f);
			brushMesh.upload(brushbuf);
			brush_changed = false;
		}
		colorProg.bind();
		brushMesh.draw();
		
		uni.MVP = VP;
		uni.light_pos.w = 1.0f;
		unibuf.upload(&uni, sizeof(uni));
		if(glfwGetKey(window.getWindow(), GLFW_KEY_Q) && waitcounter < 0){
			waitcounter = 10;
			insert(&csgroot, CSG(at, vec3(bsize), brush, bsize, 1));
			edit = true;
		}
		
		if(edit){
			vb.clear();
			std::vector<CSGOctNode*> nodes;
			collect(nodes, &csgroot);	
			for(CSGOctNode* np : nodes){
				np->remesh(spu);
				vb.insert(end(vb), begin(np->vb), end(np->vb));
			}
			mesh.upload(vb);
			edit = false;
		}
		
		//timer.begin();
		mesh.draw();
		//timer.endPrint();
        window.swap();
    }
    return 0;
}

