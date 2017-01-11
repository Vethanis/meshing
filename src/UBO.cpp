#include "myglheaders.h"
#include "UBO.h"
#include "debugmacro.h"

static unsigned binding_tail = 0;

UBO::UBO(void* ptr, size_t size, const char* name, unsigned* programs, int num_progs){
    glGenBuffers(1, &id);MYGLERRORMACRO
    glBindBuffer(GL_UNIFORM_BUFFER, id);MYGLERRORMACRO
    glBufferData(GL_UNIFORM_BUFFER, size, ptr, GL_STATIC_DRAW);MYGLERRORMACRO

    for(int i = 0; i < num_progs; i++){
        unsigned idx = glGetUniformBlockIndex(programs[i], name);
        glUniformBlockBinding(programs[i], idx, binding_tail);
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, binding_tail, id);MYGLERRORMACRO
    
    binding_tail++;
}
UBO::~UBO(){
    glDeleteBuffers(1, &id);MYGLERRORMACRO
}
void UBO::upload(void* ptr, size_t size){
    glBindBuffer(GL_UNIFORM_BUFFER, id);MYGLERRORMACRO
    glBufferData(GL_UNIFORM_BUFFER, size, ptr, GL_STATIC_DRAW);MYGLERRORMACRO
}
