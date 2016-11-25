#include "shader.h"

#include "myglheaders.h"
#include "stdio.h"

unsigned createShader(const char* src, int type){
    unsigned handle = glCreateShader(type);
    glShaderSource(handle, 1, &src, NULL);
    glCompileShader(handle);

    int result = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);

    if(!result){
        int loglen = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglen);
        char* log = new char[loglen + 1];
        glGetShaderInfoLog(handle, loglen, NULL, log);
        log[loglen] = 0;
        puts(log);
        delete[] log;
    }

    return handle;
}

void deleteShader(unsigned id){
    glDeleteShader(id);
}
