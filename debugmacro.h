#ifndef DEBUGMACRO_H
#define DEBUGMACRO_H

#ifdef DEBUG

#include "myglheaders.h"
#include "stdio.h"
#include <iostream>

#define MYGLERRORMACRO {    \
    GLenum err = GL_NO_ERROR; \
    while((err = glGetError()) != GL_NO_ERROR){ \
        switch(err){    \
            case 0x0500:    \
                printf("invalid enumeration at ");  \
                PRINTLINEMACRO  \
                break;  \
            case 0x501: \
                printf("invalid value at ");    \
                PRINTLINEMACRO  \
                break;  \
            case 0x502: \
                printf("invalid operation at ");    \
                PRINTLINEMACRO  \
                break;  \
            case 0x503: \
                printf("stack overflow at ");   \
                PRINTLINEMACRO  \
                break;  \
            case 0x504: \
                printf("stack underflow at ");  \
                PRINTLINEMACRO  \
                break;  \
            case 0x505: \
                printf("out of memory at ");    \
                PRINTLINEMACRO  \
                break;  \
            case 0x506: \
                printf("invalid framebuffer operation at ");    \
                PRINTLINEMACRO  \
                break;  \
            case 0x507: \
                printf("context lost at "); \
                PRINTLINEMACRO  \
                break;  \
            case 0x508: \
                printf("table too large at ");  \
                PRINTLINEMACRO  \
                break;  \
        }   \
    }   \
}

#define PRINTLINEMACRO {    \
    printf("%d in %s\n", __LINE__, __FILE__); \
}   

#else
#define MYGLERRORMACRO ;
#define PRINTLINEMACRO ;
#endif //ifdef DEBUG

#include "glm/glm.hpp"

inline void print(const glm::vec2& v){
    #ifdef DEBUG
    printf("(%.3f, %.3f)\n", v.x, v.y);
    #endif
}

inline void print(const glm::vec3& v){
    #ifdef DEBUG
    printf("(%.3f, %.3f, %.3f)\n", v.x, v.y, v.z);
    #endif
}

inline void print(const glm::vec4& v){
    #ifdef DEBUG
    printf("(%.3f, %.3f, %.3f, %.3f)\n", v.x, v.y, v.z, v.w);
    #endif
}

inline void print(const glm::mat3& m){
    #ifdef DEBUG
    print(m[0]);
    print(m[1]);
    print(m[2]);
    #endif
}

inline void print(const glm::mat4& m){
    #ifdef DEBUG
    print(m[0]);
    print(m[1]);
    print(m[2]);
    print(m[3]);
    #endif
}

#endif //debugmacro_h
