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
    vec4 mat_params;

    Uniforms(){
        mat_params = vec4(0.5);
    }
};

class Worker{
    static constexpr u32 num_threads = 4;
    Array<thread, num_threads> threads;
    hybrid_mutex thread_mtex;
    condition_variable_any cvar;
    CircularQueue<CSG, 128> queue;
    oct::OctScene* scene;
    bool run;
    inline bool empty(){
        return queue.empty();
    }
    inline bool full(){
        return queue.full();
    }
    void kernel(int id){
        while(run){
            unique_lock<hybrid_mutex> lock(thread_mtex);
            cvar.wait(lock, [this]{return !Worker::empty() || scene->hasWork() || !run;});
            while(run && !queue.empty()){
                scene->insert(queue.pop(), id);
            }
            if(run){
                scene->remesh(id);
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
    Worker(oct::OctScene& _scene) : 
        scene(&_scene),
        run(true){
        for(int i = 0; i < num_threads; i++){
            threads.grow() = thread(&Worker::kernel, this, i);
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

static oct::OctScene scene;

int main(int argc, char* argv[]){
	srand(unsigned(time(0)));

    int WIDTH = int(1920.0f * 1.5f);
    int HEIGHT = int(1080.0f * 1.5f);

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
    colorProg.addShader("assets/vert.glsl", GL_VERTEX_SHADER);
    colorProg.addShader("assets/frag.glsl", GL_FRAGMENT_SHADER);
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

    input.poll();
    unsigned i = 0;
    float t = (float)glfwGetTime();
    int waitcounter = 2;

    Worker worker(scene);

    Mesh brush_mesh;
    VertexBuffer brush_vb;

    constexpr int shape_count = 2;
    int shape_idx = 0;
    shape_t shapes[shape_count] = {
        SPHERE,
        BOX,
    };

    CSG cur_edit;
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
        waitcounter--;

        glm::vec3 at = camera.getEye() + (1.0f + (2.0f * cur_edit.param.size.x)) * camera.getAxis();
        cur_edit.param.center = at;

        uni.MVP = camera.getVP();
        uni.eye = vec4(camera.getEye(), 0.0f);
        if(glfwGetKey(window.getWindow(), GLFW_KEY_E)){
            uni.light_pos = vec4(-1.0f * camera.getAxis(), 0.0f);
        }
		uni.seed.x = rand();
        unibuf.upload(&uni, sizeof(uni));

        if(glfwGetKey(window.getWindow(), GLFW_KEY_UP)){ cur_edit.param.size *= 1.1f;}
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_DOWN)){ cur_edit.param.size *= 0.9f;}
        if(glfwGetKey(window.getWindow(), GLFW_KEY_RIGHT)){ 
            cur_edit.param.smoothness *= 1.1f; 
            cur_edit.param.smoothness = glm::clamp(
                cur_edit.param.smoothness, 
                0.0f, 0.1f);
        }
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_LEFT)){ 
            cur_edit.param.smoothness *= 0.9f;
            cur_edit.param.smoothness = glm::clamp(
                cur_edit.param.smoothness, 
                0.0f, 0.1f);
        }

	    if (glfwGetKey(window.getWindow(), GLFW_KEY_1) && waitcounter < 0) { 
            cur_edit.type.shape = shapes[++shape_idx % shape_count];
            waitcounter = 10;
        }

	    if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_7)) { cur_edit.param.color.x *= 1.1f;}
	    else if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_4)) { cur_edit.param.color.x *= 0.9f;}
	    if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_8)) { cur_edit.param.color.y *= 1.1f;}
	    else if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_5)) { cur_edit.param.color.y *= 0.9f;}
	    if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_9)) { cur_edit.param.color.z *= 1.1f;}
	    else if (glfwGetKey(window.getWindow(), GLFW_KEY_KP_6)) { cur_edit.param.color.z *= 0.9f;}
        cur_edit.param.color = glm::clamp(glm::vec3(0.0f), glm::vec3(1.0f), cur_edit.param.color);

        if(glfwGetKey(window.getWindow(), GLFW_KEY_F1)){ cur_edit.param.roughness *= 1.1f; }
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_F2)){ cur_edit.param.roughness *= 0.9f; }
        if(glfwGetKey(window.getWindow(), GLFW_KEY_F3)){ cur_edit.param.metalness *= 1.1f; }
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_F4)){ cur_edit.param.metalness *= 0.9f; }
        cur_edit.param.roughness = glm::clamp(cur_edit.param.roughness, 0.05f, 0.95f);
        cur_edit.param.metalness = glm::clamp(cur_edit.param.metalness, 0.05f, 0.95f);

        {   // rebuild brush at new looking point
            brush_vb.clear();
            fillCells(brush_vb, cur_edit, cur_edit.param.center, cur_edit.param.size.x);
            brush_mesh.update(brush_vb);
        }

        if(input.leftMouseDown() && waitcounter < 0){
            cur_edit.type.blend = blend_t::SMOOTH_ADD;
            worker.insert(cur_edit);
            waitcounter = 1 + int(cur_edit.param.size.x * 10.0f);
        }
        else if(input.rightMouseDown() && waitcounter < 0){
            cur_edit.type.blend = blend_t::SMOOTH_SUB;
            worker.insert(cur_edit);
            waitcounter = 1 + int(cur_edit.param.size.x * 10.0f);
        }

        brush_mesh.draw();
        scene.update();
        scene.draw();

        window.swap();
    }

    brush_mesh.destroy();

    return 0;
}
