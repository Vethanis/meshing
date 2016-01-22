#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "myglheaders.h"

class Shader{
    GLuint handle;
    void loadSourceFile(const char* path, std::string& target_on_stack);
public:
    Shader(const char* src_path, GLenum type);
    ~Shader();
    inline GLuint getHandle(){return handle;};
};
#endif
