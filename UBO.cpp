#include "myglheaders.h"
#include "UBO.h"
#include "debugmacro.h"


UBO::UBO(void* ptr, size_t size, unsigned binding){
	glGenBuffers(1, &id);MYGLERRORMACRO
	glBindBuffer(GL_UNIFORM_BUFFER, id);MYGLERRORMACRO
	glBufferData(GL_UNIFORM_BUFFER, size, ptr, GL_STATIC_DRAW);MYGLERRORMACRO
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);MYGLERRORMACRO
}
UBO::~UBO(){
	glDeleteBuffers(1, &id);MYGLERRORMACRO
}
void UBO::upload(void* ptr, size_t size){
	glBindBuffer(GL_UNIFORM_BUFFER, id);MYGLERRORMACRO
	glBufferData(GL_UNIFORM_BUFFER, size, ptr, GL_STATIC_DRAW);MYGLERRORMACRO
}
