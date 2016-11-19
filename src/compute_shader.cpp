#include "myglheaders.h"
#include "compute_shader.h"
#include "debugmacro.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include "glm/gtc/type_ptr.hpp"

ComputeShader::ComputeShader(const std::string& filename){
    progid = glCreateProgram();
    shaderid = glCreateShader(GL_COMPUTE_SHADER);
    
    std::string source;
    std::ifstream stream(filename);
    if(!stream.is_open()){
        printf("Could not open compute shader source %s\n", filename.c_str());
        return;
    }
    
    std::string line;
    while(getline(stream, line))
        source += line + "\n";
    
    char const* src_pointer = source.c_str();
    glShaderSource(shaderid, 1, &src_pointer, NULL);
    
    GLint result = GL_FALSE;
    int infoLogLength;
    glCompileShader(shaderid);
    glGetShaderiv(shaderid, GL_COMPILE_STATUS, &result);
    if(!result){
        glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> shaderErrorMessage( std::max( infoLogLength, int(1) ) );
        glGetShaderInfoLog(shaderid, infoLogLength, NULL, &shaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &shaderErrorMessage[0]);
        glDeleteShader(shaderid);
        glDeleteProgram(progid);
        return;
    }
    
    glAttachShader(progid, shaderid);
    glLinkProgram(progid);
    glDeleteShader(shaderid);
    
    result = GL_FALSE;
    glGetProgramiv(progid, GL_LINK_STATUS, &result);
    if(!result){
        glGetProgramiv(progid, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> shaderErrorMessage( std::max( infoLogLength, int(1) ) );
        glGetProgramInfoLog(progid, infoLogLength, NULL, &shaderErrorMessage[0]);
        fprintf(stdout, "%s\n", &shaderErrorMessage[0]);
        glDeleteProgram(progid);
        return;
    }
    
    MYGLERRORMACRO
}
ComputeShader::~ComputeShader(){
    glDeleteProgram(progid);
    MYGLERRORMACRO
}
void ComputeShader::bind(){
    glUseProgram(progid);
    MYGLERRORMACRO
}
void ComputeShader::call(unsigned x, unsigned y, unsigned z){
    glDispatchCompute(x, y, z);
}

int ComputeShader::getUniformLocation(const std::string& name){
    auto iter = uniforms.find(name);
    if(iter == end(uniforms)){
        const int v = glGetUniformLocation(progid, name.c_str());
        uniforms[name] = v;
        return v;
    }
    return iter->second;
}

void ComputeShader::setUniform(const std::string& name, const glm::vec2& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform2fv(location, 1, glm::value_ptr(v));
}
void ComputeShader::setUniform(const std::string& name, const glm::vec3& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform3fv(location, 1, glm::value_ptr(v));
}
void ComputeShader::setUniform(const std::string& name, const glm::vec4& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform4fv(location, 1, glm::value_ptr(v));
}
void ComputeShader::setUniform(const std::string& name, const glm::mat3& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniformMatrix3fv(location, 1, false, glm::value_ptr(v));
}
void ComputeShader::setUniform(const std::string& name, const glm::mat4& v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(v));
}
void ComputeShader::setUniformInt(const std::string& name, const int v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform1i(location, v);
}
void ComputeShader::setUniformFloat(const std::string& name, const float v){
    const int location = getUniformLocation(name);
    if (location == -1)
        return;
    glUniform1f(location, v);
}

