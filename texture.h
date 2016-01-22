#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "glm/glm.hpp"

enum TEXTURETYPE{
	COLOR,
	UBYTE,
	UBYTE2,
	UBYTE3,
	SBYTE,
	SBYTE2,
	SBYTE3,
	SINT,
	SINT2,
	SINT3,
	FLOAT,
	FLOAT2,
	FLOAT3
};

class GLProgram;

class Texture{
	unsigned tex_id, width, height;
	TEXTURETYPE type;
public:
	Texture(int w, int h, TEXTURETYPE t);
	~Texture();
	void bind(int channel, const std::string& uniform_name, GLProgram& prog);
	void setCSBinding(int binding);
	unsigned getID(){return tex_id;};
	unsigned getWidth(){return width;};
	unsigned getHeight(){return height;};
	void setPixel(glm::ivec2 cr, float d);
};

#endif
