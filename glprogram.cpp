#include "glprogram.h"
#include "shader.h"
#include "myglheaders.h"
#include "debugmacro.h"
#include <vector>
#include <algorithm>
#include "glm/gtc/type_ptr.hpp"

using namespace std;

GLProgram::GLProgram(const char* vert_path, const char* frag_path){
    std::string vertSrc = vert_path;
    Shader vertShader(vertSrc.c_str(), GL_VERTEX_SHADER);
    std::string fragSrc = frag_path;
    Shader fragShader(fragSrc.c_str(), GL_FRAGMENT_SHADER);
    
    progHandle = glCreateProgram();
    glAttachShader(progHandle, vertShader.getHandle());
    glAttachShader(progHandle, fragShader.getHandle());

    glLinkProgram(progHandle);

    GLint result;
    int infoLogLength;

    glGetProgramiv(progHandle, GL_LINK_STATUS, &result);
    if(!result){
		glGetProgramiv(progHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> ProgramErrorMessage( std::max(infoLogLength, int(1)) );
		glGetProgramInfoLog(progHandle, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}
    MYGLERRORMACRO
}

GLProgram::~GLProgram(){
    glDeleteProgram(progHandle);
    MYGLERRORMACRO
}

void GLProgram::bind(){
    glUseProgram(progHandle);
    MYGLERRORMACRO
}

int GLProgram::getUniformLocation(const std::string& name){
	auto iter = uniforms.find(name);
	if(iter == end(uniforms)){
		const int v = glGetUniformLocation(progHandle, name.c_str());
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

