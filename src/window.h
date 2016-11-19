#ifndef WINDOW_H
#define WINDOW_H

#include <string>

struct GLFWwindow;

class Window{
private:
    GLFWwindow* window;
    static void error_callback(int error, const char* description);
public:
    Window(int width, int height, int major_ver, int minor_ver, const std::string& title);
    ~Window();
    inline GLFWwindow* getWindow(){ return window; };
    bool open();
    void swap();

};
#endif
