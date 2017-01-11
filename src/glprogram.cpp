#include "myglheaders.h"

#include "glprogram.h"

#include "shader.h"
#include "loadfile.h"
#include "debugmacro.h"
#include "glm/gtc/type_ptr.hpp"
#include "stdio.h"

using namespace std;

GLProgram::GLProgram(){
    id = glCreateProgram();
}

GLProgram::~GLProgram(){
    glDeleteProgram(id);
    MYGLERRORMACRO
}

void GLProgram::addShader(const char* path, int type){
    char* src = load_file(path);
    unsigned handle = createShader(src, type);
    glAttachShader(id, handle);
    delete[] src;
}

void GLProgram::link(){
    glLinkProgram(id);

    int result = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &result);
    if(!result){
        int loglen = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglen);
        char* log = new char[loglen + 1];
        glGetProgramInfoLog(id, loglen, NULL, log);
        log[loglen] = 0;
        puts(log);
        delete[] log;
    }
}

void GLProgram::bind(){
    glUseProgram(id);
    MYGLERRORMACRO
}

int GLProgram::getUniformLocation(const std::string& name){
    auto iter = uniforms.find(name);
    if(iter == end(uniforms)){
        const int v = glGetUniformLocation(id, name.c_str());
        uniforms[name] = v;
        return v;
    }
    return iter->second;
}

void GLProgram::setUniform(const std::string& name, const glm::vec2& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform2fv(location, 1, glm::value_ptr(v));
}
void GLProgram::setUniform(const std::string& name, const glm::vec3& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform3fv(location, 1, glm::value_ptr(v));
}
void GLProgram::setUniform(const std::string& name, const glm::vec4& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform4fv(location, 1, glm::value_ptr(v));
}
void GLProgram::setUniform(const std::string& name, const glm::mat3& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniformMatrix3fv(location, 1, false, glm::value_ptr(v));
}
void GLProgram::setUniform(const std::string& name, const glm::mat4& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(v));
}
void GLProgram::setUniformInt(const std::string& name, const int v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform1i(location, v);
}
void GLProgram::setUniformFloat(const std::string& name, const float v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform1f(location, v);
}
