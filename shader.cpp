#include "shader.h"
#include <vector>
#include <algorithm>
#include <fstream>

#include "myglheaders.h"
#include "debugmacro.h"

Shader::Shader(const char* src_path, GLenum type){
    std::string src = "";
    loadSourceFile(src_path, src);
    char const* src_pointer = src.c_str();
    handle = glCreateShader(type);
    glShaderSource(handle, 1, &src_pointer, NULL);

    GLint result = GL_FALSE;
    int infoLogLength;
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
    if(!result){
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> shaderErrorMessage( std::max( infoLogLength, int(1) ) );
		glGetShaderInfoLog(handle, infoLogLength, NULL, &shaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &shaderErrorMessage[0]);
	}
    MYGLERRORMACRO
}

Shader::~Shader(){
    glDeleteShader(handle);
    MYGLERRORMACRO
}

void Shader::loadSourceFile(const char* src_path, std::string& src_on_stack){
    std::ifstream stream(src_path, std::ios::in);
    if(stream.is_open()){
        std::string line;
        while(getline(stream, line))
            src_on_stack += line + "\n";//"\n" + line;
        stream.close();
    }
}
