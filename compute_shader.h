#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class ComputeShader{
    unsigned progid, shaderid;
    std::unordered_map<std::string, int> uniforms;
    int getUniformLocation(const std::string& name);
public:
    ComputeShader(const std::string& filename);
    ~ComputeShader();
    void bind();
    void call(unsigned x, unsigned y, unsigned z);
    void setUniform(const std::string& name, const glm::vec2& v);
    void setUniform(const std::string& name, const glm::vec3& v);
    void setUniform(const std::string& name, const glm::vec4& v);
    void setUniform(const std::string& name, const glm::mat3& v);
    void setUniform(const std::string& name, const glm::mat4& v);
    void setUniformInt(const std::string& name, const int v);
    void setUniformFloat(const std::string& name, const float v);
};

#endif
