#include "myglheaders.h"
#include "stdio.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "UBO.h"
#include "timer.h"
#include "octree.h"
#include <thread>
#include <condition_variable>
#include "circular_queue.h"
#include "hybrid_mutex.h"

#include "time.h"
#include <random>

using namespace std;
using namespace glm;

struct Uniforms{
    mat4 MVP;
    vec4 eye;
    vec4 light_pos;
	ivec4 seed;
};

class Worker{
    vector<thread> threads;
    hybrid_mutex thread_mtex;
    condition_variable_any cvar;
    CircularQueue<CSG, 128> queue;
    bool run;
    inline bool empty(){
        return queue.empty();
    }
    inline bool full(){
        return queue.full();
    }
    void kernel(){
        while(run){
            unique_lock<hybrid_mutex> lock(thread_mtex);
            cvar.wait(lock, [this]{return !Worker::empty() || !run;});
            while(run && !empty()){
                oct::insert(queue.pop());
            }
            if(run){
                oct::g_leafData.remesh();
            }
        }
    }
public:
	inline void quit() {
		run = false;
		cvar.notify_all();
		for(auto& i : threads){
            i.join();
        }
	}
    Worker()
        : run(true){
        for(int i = 0; i < 4; i++){
            threads.push_back(thread(&Worker::kernel, this));
        }
    }
    ~Worker(){
		quit();
    }
    inline void insert(const CSG& csg){
        if(full()){ // avoid overflow
            puts("Worker thread circular buffer overflow");
            return;
        }

        queue.push(csg);
        cvar.notify_one();
    }
};


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
	srand(unsigned(time(0)));

    int WIDTH = 1280;
    int HEIGHT = 720;

    if(argc >= 3){
        WIDTH = atoi(argv[1]);
        HEIGHT = atoi(argv[2]);
    }


    Camera camera;
    camera.resize(WIDTH, HEIGHT);
    camera.setEye(vec3(0.0f, 0.0f, 3.0f));
    camera.update();

    Window window(WIDTH, HEIGHT, 3, 3, "Meshing");
    Input input(window.getWindow());
    GLProgram colorProg;
    colorProg.addShader("vert.glsl", GL_VERTEX_SHADER);
    colorProg.addShader("frag.glsl", GL_FRAGMENT_SHADER);
    colorProg.link();
    colorProg.bind();

    oct::OctNode root(glm::vec3(0.0f), 0);

    Timer timer;

    Uniforms uni;
    uni.light_pos = vec4(-1.0f * camera.getAxis(), 0.0f);
	uni.seed.x = rand();
    UBO unibuf(&uni, sizeof(uni), "UniBlock", &colorProg.id, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    float bsize = 0.05f, smoothness = 0.333f;
    glm::vec3 color = {0.5f, 0.5f, 0.5f};

    input.poll();
    unsigned i = 0;
    float t = (float)glfwGetTime();
    int waitcounter = 2;

    Worker worker;

    Mesh brush_mesh;
    VertexBuffer brush_vb;

    CSG_Type csg_type;
    csg_type.blend = blend_t::SMOOTH_ADD;
    constexpr int shape_count = 2;
    int shape_idx = 0;
    shape_t shapes[shape_count] = {
        SPHERE,
        BOX,
    };

    while(window.open()){
        input.poll(frameBegin(i, t), camera);
        waitcounter--;

        glm::vec3 at = camera.getEye() + (1.0f + (2.0f * bsize)) * camera.getAxis();

        uni.MVP = camera.getVP();
        uni.eye = vec4(camera.getEye(), 0.0f);
        if(glfwGetKey(window.getWindow(), GLFW_KEY_E)){
            uni.light_pos = vec4(-1.0f * camera.getAxis(), 0.0f);
        }
		uni.seed.x = rand();
        unibuf.upload(&uni, sizeof(uni));

        if(glfwGetKey(window.getWindow(), GLFW_KEY_UP)){ bsize *= 1.1f;}
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_DOWN)){ bsize *= 0.9f;}
        if(glfwGetKey(window.getWindow(), GLFW_KEY_RIGHT)){ smoothness *= 1.1f; smoothness = smoothness >= 0.5f ? 0.5f : smoothness;}
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_LEFT)){ smoothness *= 0.9f;}

	    if (glfwGetKey(window.getWindow(), GLFW_KEY_1) && waitcounter < 0) { 
            csg_type.shape = shapes[++shape_idx % shape_count];
            waitcounter = 10;
        }

	    if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_7)) { color.x *= 1.1f;}
	    else if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_4)) { color.x *= 0.9f;}
	    if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_8)) { color.y *= 1.1f;}
	    else if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_5)) { color.y *= 0.9f;}
	    if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_9)) { color.z *= 1.1f;}
	    else if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_6)) { color.z *= 0.9f;}
        color = glm::clamp(glm::vec3(0.0f), glm::vec3(1.0f), color);

        {   // rebuild brush at new looking point
    	    CSG item = {
                {
                    at, 
                    vec3(bsize), 
                    color, 
                    smoothness * bsize
                },
                csg_type
            };
            brush_vb.clear();
            fillCells(brush_vb, item, at, bsize);
            brush_mesh.update(brush_vb);
        }

        if(input.leftMouseDown() && waitcounter < 0){
            csg_type.blend = blend_t::SMOOTH_ADD;
            CSG item = {
                {
                    at, 
                    vec3(bsize), 
                    color, 
                    smoothness * bsize
                },
                csg_type
            };
            worker.insert(item);
            waitcounter = 1 + int(bsize * 4.0f);
        }
        else if(input.rightMouseDown() && waitcounter < 0){
            csg_type.blend = blend_t::SMOOTH_SUB;
            CSG item = {
                {
                    at, 
                    vec3(bsize), 
                    color, 
                    smoothness * bsize
                },
                csg_type
            };
            worker.insert(item);
            waitcounter = 1 + int(bsize * 4.0f);
        }

        brush_mesh.draw();
        oct::g_leafData.update();
        oct::g_leafData.draw();

        window.swap();
    }

    brush_mesh.destroy();

    return 0;
}
