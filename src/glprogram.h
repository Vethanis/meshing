#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct GLProgram{
    std::unordered_map<std::string, unsigned> uniforms;
    unsigned id;
    int getUniformLocation(const std::string& name);
    GLProgram();
    ~GLProgram();
    void addShader(const char* path, int type);
    void link();
    void bind();
    void setUniform(const std::string& name, const glm::vec2& v);
    void setUniform(const std::string& name, const glm::vec3& v);
    void setUniform(const std::string& name, const glm::vec4& v);
    void setUniform(const std::string& name, const glm::mat3& v);
    void setUniform(const std::string& name, const glm::mat4& v);
    void setUniformInt(const std::string& name, const int v);
    void setUniformFloat(const std::string& name, const float v);
};
#endif
