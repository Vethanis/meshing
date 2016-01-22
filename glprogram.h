#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class GLProgram{
	std::unordered_map<std::string, unsigned> uniforms;
    unsigned progHandle;
    int getUniformLocation(const std::string& name);
public:
	GLProgram(const char* vert_path, const char* frag_path);
    ~GLProgram();
    void setUniform(const std::string& name, const glm::vec2& v);
    void setUniform(const std::string& name, const glm::vec3& v);
    void setUniform(const std::string& name, const glm::vec4& v);
    void setUniform(const std::string& name, const glm::mat3& v);
    void setUniform(const std::string& name, const glm::mat4& v);
    void setUniformInt(const std::string& name, const int v);
    void setUniformFloat(const std::string& name, const float v);
    void bind();
};
#endif
