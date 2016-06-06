
#include "myglheaders.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "UBO.h"
#include "mesh.h"
#include "timer.h"
#include "octree.h"
#include <thread>

using namespace std;
using namespace glm;

struct Uniforms{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
};

void remesh(OctNode* root, std::vector<CSG*>* insertQueue, VertexBuffer* vb, bool* done, float spu){
	for(CSG* i : *insertQueue){
		root->insert(i);
	}
	insertQueue->clear();
	vb->clear();
	root->remesh(*vb, spu);
	*done = true;
}


float frameBegin(unsigned& i, float& t){
    float dt = (float)glfwGetTime() - t;
    t += dt;
    i++;
    if(t >= 3.0){
    	float ms = (t / i) * 1000.0f;
        printf("ms: %.6f, FPS: %.3f\n", ms, i / t);
        i = 0;
        t = 0.0;
        glfwSetTime(0.0);
    }
    return dt;
}

int main(int argc, char* argv[]){
	if(argc != 3){
        printf("Usage: meshing.exe <width> <height>\n");
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
	
	OctNode root(glm::vec3(0.0f), 8.0f, 0);
	Mesh rootMesh;
	rootMesh.init();
	Mesh brushMesh;
	brushMesh.init();
	VertexBuffer vb;
	
	Timer timer;
	
	Uniforms uni;
	uni.light_pos = vec4(5.0f, 5.0f, 5.0f, 0.0f);
	UBO unibuf(&uni, sizeof(uni), 0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	float bsize = 0.2f;
	
	input.poll();
    unsigned i = 0;
    float t = (float)glfwGetTime();
    int waitcounter = 10;
    bool brush_changed = true;
    bool box = false;
    bool edit = false;
    float spu = 7.0f;
    bool remeshed = false;
    std::vector<CSG*> workQueue[2];
    thread* worker = nullptr;
    int wqid = 0;
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
    	waitcounter--;
    	
    	glm::vec3 at = camera.getEye() + 3.0f * camera.getAxis();
        
        mat4 brushMat = glm::translate(glm::mat4(), at);
        mat4 VP = camera.getVP();
        uni.MVP = VP * brushMat;
        uni.eye = vec4(camera.getEye(), 0.0f);
		uni.light_pos = vec4(camera.getEye(), 0.0f);
		unibuf.upload(&uni, sizeof(uni));
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_UP)){ bsize *= 1.1f; brush_changed = true;}
		else if(glfwGetKey(window.getWindow(), GLFW_KEY_DOWN)){ bsize *= 0.9f; brush_changed = true;}
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_1) && waitcounter < 0){ box = !box; waitcounter = 10; brush_changed = true;}
		if(glfwGetKey(window.getWindow(), GLFW_KEY_3)){ spu *= 1.01f;}
		else if(glfwGetKey(window.getWindow(), GLFW_KEY_4)){spu *= 0.99f;}
		
		if(brush_changed){
			size_t sz = (vb.size()) ? (vb.size() >> 1) : 512;
			vb.clear();
			vb.reserve(sz);
			if(box)
				fillCells(vb, CSG(vec3(0.0f), vec3(bsize), &BOXADD, bsize, 1), spu);
			else
				fillCells(vb, CSG(vec3(0.0f), vec3(bsize), &SPHEREADD, bsize, 1), spu);
			brushMesh.update(vb);
			brush_changed = false;
		}
		
		colorProg.bind();
		brushMesh.draw();
		
		uni.MVP = VP;
		unibuf.upload(&uni, sizeof(uni));
		
		if(input.leftMouseDown() && waitcounter < 0){
			SDF_Base* type = &SPHERESADD;
			if(box) type = &BOXSADD;
			workQueue[wqid].push_back(new CSG(at, vec3(bsize), type, bsize, 1));
			waitcounter = (int)(bsize * 30.0f);
			edit = true;
			print(at);
		}
		else if(input.rightMouseDown() && waitcounter < 0){
			SDF_Base* type = &SPHERESUB;
			if(box) type = &BOXSUB;
			workQueue[wqid].push_back(new CSG(at, vec3(bsize), type, bsize, 1)); 
			waitcounter = (int)(bsize * 30.0f);
			edit = true;
			print(at);
		}
		
		if(edit && !worker){
			worker = new thread(&remesh, &root, &workQueue[wqid], &(root.vb), &remeshed, spu);
			wqid = (wqid+1)%2;
			edit = false;
		}
		
		if(remeshed){
			worker->join();
			delete worker;
			worker = nullptr;
			rootMesh.update(root.vb);
			remeshed = false;
		}
		
		//timer.begin();
		
		rootMesh.draw();
		
		//timer.endPrint();
        window.swap();
    }
    if(worker)worker->join();
    delete worker;
    rootMesh.destroy();
    brushMesh.destroy();
    return 0;
}

