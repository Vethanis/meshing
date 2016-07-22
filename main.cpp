
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

using namespace std;
using namespace glm;

struct Uniforms{
    mat4 MVP;
    vec4 eye;
    vec4 light_pos;
};

class Worker{
    thread m_thread;
    mutex thread_mtex, vb_mtex;
    condition_variable cvar;
    VertexBuffer vb;
    OctNode* root;
    CSG* buffer[8];
    atomic<unsigned char> head, tail;
    float spu;
    bool run;
    inline bool empty(){ 
        return head.load(memory_order_relaxed) == tail.load(memory_order_relaxed); 
    }
    inline unsigned char next(atomic<unsigned char>& counter){
        return (counter.load(memory_order_relaxed) + 1) & 7;
    }
    inline unsigned char current(atomic<unsigned char>& counter){
        return counter.load(memory_order_relaxed);
    }
    void kernel(){
        while(run){
            unique_lock<mutex> lock(thread_mtex);
            cvar.wait(lock, [this]{return !Worker::empty() || !run;});
            while(!empty()){
                root->insert(buffer[current(head)]);
                head = next(head);
            }
            lock_guard<mutex> vblock(vb_mtex);
            vb.clear();
            root->remesh(vb, spu);
        }
    }
public:
    Worker(OctNode* _root, float _spu) 
        : root(_root), spu(_spu), head(0), tail(0), run(true){
        m_thread = thread(&Worker::kernel, this);
    }
    inline void insert(CSG* csg, float _spu){
        if(next(tail) == current(head)){ // avoid overflow
            puts("OVERFLOW!");
            delete csg;
            return;
        }
        
        buffer[tail] = csg;
        spu = _spu;
        tail = next(tail);
        cvar.notify_one();
    }
    inline void pull(Mesh& mesh){
        if(!vb.size())return;
        unique_lock<mutex> vblock(vb_mtex, defer_lock);
        if(vblock.try_lock()){
            mesh.update(vb);
            vb.clear();
            vblock.unlock();
        }
    }
    inline void quit(){
        run = false;
        thread_mtex.unlock();
        vb_mtex.unlock();
        cvar.notify_all();
        m_thread.join();
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
    
    Worker worker(&root, spu);
    
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
            vb.clear();
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
            worker.insert(new CSG(at, vec3(bsize), type, bsize, 1), spu);
            waitcounter = (int)(bsize * 20.0f);
        }
        else if(input.rightMouseDown() && waitcounter < 0){
            SDF_Base* type = &SPHERESUB;
            if(box) type = &BOXSUB;
            worker.insert(new CSG(at, vec3(bsize), type, bsize, 1), spu); 
            waitcounter = (int)(bsize * 20.0f);
        }
        
        worker.pull(rootMesh);
        
        //timer.begin();
        
        rootMesh.draw();
        
        //timer.endPrint();
        window.swap();
    }
    
    worker.quit();
    
    rootMesh.destroy();
    brushMesh.destroy();
    return 0;
}

