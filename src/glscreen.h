#ifndef GLSCREEN_H
#define GLSCREEN_H

class GLScreen{
    unsigned vao, vbo;
public:
    GLScreen();
    ~GLScreen();
    void draw();
};

#endif
