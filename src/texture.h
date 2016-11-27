#ifndef TEXTURE_H
#define TEXTURE_H

#include "myglheaders.h"
#include "glm/glm.hpp"
#include "debugmacro.h"
#include "glprogram.h"


template<typename T, int FullType, int Channels, int ComponentType>
struct Texture{
    int width, height;
    unsigned handle;
    Texture(int w, int h, bool mip=false) : width(w), height(h){
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);
        if(mip){
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    MYGLERRORMACRO
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);    MYGLERRORMACRO
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);    MYGLERRORMACRO
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    MYGLERRORMACRO
        }
        else{
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    MYGLERRORMACRO
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    MYGLERRORMACRO
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    MYGLERRORMACRO
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);    MYGLERRORMACRO
        }
        glTexImage2D(GL_TEXTURE_2D, 0, FullType, width, height, 0, Channels, ComponentType, NULL);    MYGLERRORMACRO
        if(mip)
            glGenerateMipmap(GL_TEXTURE_2D);    MYGLERRORMACRO

        glBindTexture(GL_TEXTURE_2D, 0);
        MYGLERRORMACRO
    }
    ~Texture(){
        glDeleteTextures(1, &handle);    MYGLERRORMACRO
    }
    void bind(int channel, const char* uname, GLProgram& prog){
        glActiveTexture(GL_TEXTURE0 + channel);
        MYGLERRORMACRO
        glBindTexture(GL_TEXTURE_2D, handle);
        MYGLERRORMACRO
        prog.setUniformInt(uname, channel);
    }
    void setCSBinding(int binding){
        glBindImageTexture(0, handle, 0, GL_FALSE, 0, GL_READ_WRITE, FullType);
        MYGLERRORMACRO
    }
    void setPixel(glm::ivec2 cr, const void* p){
        glTextureSubImage2D(handle, 0, cr.x, cr.y, 1, 1, Channels, ComponentType, p);
        MYGLERRORMACRO
    }
};

typedef Texture<float, GL_R32F, GL_RED, GL_FLOAT> Texture1f;
typedef Texture<glm::vec2, GL_RG32F, GL_RG, GL_FLOAT> Texture2f;
typedef Texture<glm::vec3, GL_RGB32F, GL_RGB, GL_FLOAT> Texture3f;
typedef Texture<glm::vec4, GL_RGBA32F, GL_RGBA, GL_FLOAT> Texture4f;

typedef Texture<int, GL_R32I, GL_RED, GL_INT> Texture1i;
typedef Texture<glm::ivec2, GL_RG32I, GL_RG, GL_INT> Texture2i;
typedef Texture<glm::ivec3, GL_RGB32I, GL_RGB, GL_INT> Texture3i;
typedef Texture<glm::ivec4, GL_RGBA32I, GL_RGBA, GL_INT> Texture4i;

typedef Texture<unsigned, GL_R32UI, GL_RED, GL_UNSIGNED_INT> Texture1u;
typedef Texture<glm::uvec2, GL_RG32UI, GL_RG, GL_UNSIGNED_INT> Texture2u;
typedef Texture<glm::uvec3, GL_RGB32UI, GL_RGB, GL_UNSIGNED_INT> Texture3u;
typedef Texture<glm::uvec4, GL_RGBA32UI, GL_RGBA, GL_UNSIGNED_INT> Texture4u;

struct cvec2{char x, y;};
struct cvec3{char x, y, z;};
struct cvec4{char x, y, z, w;};

typedef Texture<char, GL_R8I, GL_RED, GL_BYTE> Texture1c;
typedef Texture<cvec2, GL_RG8I, GL_RG, GL_BYTE> Texture2c;
typedef Texture<cvec3, GL_RGB8I, GL_RGB, GL_BYTE> Texture3c;
typedef Texture<cvec4, GL_RGBA8I, GL_RGBA, GL_BYTE> Texture4c;

struct ucvec2{unsigned char x, y;};
struct ucvec3{unsigned char x, y, z;};
struct ucvec4{unsigned char x, y, z, w;};

typedef Texture<unsigned char, GL_R8UI, GL_RED, GL_UNSIGNED_BYTE> Texture1uc;
typedef Texture<ucvec2, GL_RG8UI, GL_RG, GL_UNSIGNED_BYTE> Texture2uc;
typedef Texture<ucvec3, GL_RGB8UI, GL_RGB, GL_UNSIGNED_BYTE> Texture3uc;
typedef Texture<ucvec4, GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_BYTE> Texture4uc;

#endif
