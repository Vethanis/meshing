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
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "vsrc.h"
#include "fsrc.h"
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
    thread m_thread;
    mutex thread_mtex;
    condition_variable cvar;
    oct::OctNode* root;
    CSG* buffer[64];
    atomic<unsigned char> head, tail;
    bool run;
    inline bool empty(){
        return head.load(memory_order_relaxed) == tail.load(memory_order_relaxed);
    }
    inline unsigned char next(atomic<unsigned char>& counter){
        return (counter.load(memory_order_relaxed) + 1) & 63;
    }
    inline unsigned char current(atomic<unsigned char>& counter){
        return counter.load(memory_order_relaxed);
    }
    void kernel(){
        while(run){
            unique_lock<mutex> lock(thread_mtex);
            cvar.wait(lock, [this]{return !Worker::empty() || !run;});
			if (!run)return;
            while(!empty()){
                root->insert(buffer[current(head)]);
                head = next(head);
            }
            oct::remesh_nodes();
        }
    }
public:
	inline void quit() {
		run = false;
		cvar.notify_all();
		m_thread.join();
	}
    Worker(oct::OctNode* _root)
        : root(_root), head(0), tail(0), run(true){
        m_thread = thread(&Worker::kernel, this);
    }
    ~Worker(){
		quit();
    }
    inline void insert(CSG* csg){
        if(next(tail) == current(head)){ // avoid overflow
            puts("Worker thread circular buffer overflow");
            delete csg;
            return;
        }

        buffer[tail] = csg;
        tail = next(tail);
        cvar.notify_one();
    }
    inline void pull(){
        oct::upload_meshes();
        oct::draw_points();
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

    Window window(WIDTH, HEIGHT, 4, 3, "Meshing");
    Input input(window.getWindow());
    GLProgram colorProg(VERTSRC, FRAGSRC);
    colorProg.bind();

    oct::OctNode root(glm::vec3(0.0f), 0);
    Mesh brushMesh;
    brushMesh.init();
    VertexBuffer vb;

    Timer timer;

    Uniforms uni;
    uni.light_pos = vec4(-1.0f * camera.getAxis(), 0.0f);
	uni.seed.x = rand();
    UBO unibuf(&uni, sizeof(uni), 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    float bsize = 0.05f;

    input.poll();
    unsigned i = 0;
    float t = (float)glfwGetTime();
    int waitcounter = 10;
    bool brush_changed = true;
    bool box = false;
    bool edit = false;

    Worker worker(&root);

    while(window.open()){
        input.poll(frameBegin(i, t), camera);
        waitcounter--;

        glm::vec3 at = camera.getEye() + 3.0f * camera.getAxis();

        uni.MVP = camera.getVP();
        uni.eye = vec4(camera.getEye(), 0.0f);
        if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
            uni.light_pos = vec4(-1.0f * camera.getAxis(), 0.0f);
		uni.seed.x = rand();
        unibuf.upload(&uni, sizeof(uni));

        if(glfwGetKey(window.getWindow(), GLFW_KEY_UP)){ bsize *= 1.1f; brush_changed = true;}
        else if(glfwGetKey(window.getWindow(), GLFW_KEY_DOWN)){ bsize *= 0.9f; brush_changed = true;}

	    if (glfwGetKey(window.getWindow(), GLFW_KEY_1) && waitcounter < 0) { box = !box; brush_changed = true; waitcounter = 10;}

        {
            vb.clear();
    	    CSG item(at, vec3(bsize), box ? BOXADD : SPHEREADD, i);
            fillCells(vb, item, at, bsize);
            brushMesh.update(vb);
            brush_changed = false;
        }

        if(input.leftMouseDown() && waitcounter < 0){
            worker.insert(new CSG(at, vec3(bsize), box ? BOXSADD : SPHERESADD, i));
            waitcounter = int(bsize * 10.0f);
        }
        else if(input.rightMouseDown() && waitcounter < 0){
            worker.insert(new CSG(at, vec3(bsize), box ? BOXSSUB : SPHERESSUB, i));
            waitcounter = int(bsize * 10.0f);;
        }

        brushMesh.draw();
        worker.pull();

        window.swap();
    }

    brushMesh.destroy();
    return 0;
}
