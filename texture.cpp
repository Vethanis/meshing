#include "myglheaders.h"
#include "texture.h"
#include "debugmacro.h"
#include "glprogram.h"


Texture::Texture(int w, int h, TEXTURETYPE t){
	width = w; height = h; type = t;
	
	glGenTextures(1, &tex_id);	MYGLERRORMACRO
	glBindTexture(GL_TEXTURE_2D, tex_id);	MYGLERRORMACRO
	
	if(type == COLOR){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	MYGLERRORMACRO
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO
		glGenerateMipmap(GL_TEXTURE_2D);	MYGLERRORMACRO
	}
	else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	MYGLERRORMACRO
	
		switch(type){
		case COLOR:
			break;
		case UBYTE:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case UBYTE2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8UI, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case UBYTE3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8UI, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SBYTE:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, width, height, 0, GL_RED, GL_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SBYTE2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8I, width, height, 0, GL_RG, GL_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SBYTE3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8I, width, height, 0, GL_RGB, GL_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SINT:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED, GL_INT, NULL);	MYGLERRORMACRO}
			break;
		case SINT2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, width, height, 0, GL_RG, GL_INT, NULL);	MYGLERRORMACRO}
			break;
		case SINT3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32I, width, height, 0, GL_RGB, GL_INT, NULL);	MYGLERRORMACRO}
			break;
		case FLOAT:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);	MYGLERRORMACRO}
			break;
		case FLOAT2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, NULL);	MYGLERRORMACRO}
			break;
		case FLOAT3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);	MYGLERRORMACRO}
			break;
		}
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);	
	MYGLERRORMACRO
}

Texture::~Texture(){
	glDeleteTextures(1, &tex_id);	MYGLERRORMACRO
}

void Texture::bind(int channel, const std::string& uname, GLProgram& prog){
	glActiveTexture(GL_TEXTURE0 + channel);
	MYGLERRORMACRO
	glBindTexture(GL_TEXTURE_2D, tex_id);
	MYGLERRORMACRO
	prog.setUniformInt(uname, channel);
}


void Texture::setCSBinding(int binding){
	/*unsigned val;
		switch(type){
		case COLOR:{val=GL_RGB8UI;	}
			break;
		case UBYTE:{val=GL_R8UI;	}
			break;
		case UBYTE2:{val=GL_RG8UI;	}
			break;
		case UBYTE3:{val=GL_RGB8UI;	}
			break;
		case SBYTE:{val=GL_R8I;		}
			break;
		case SBYTE2:{val=GL_RG8I;	}
			break;
		case SBYTE3:{val=GL_RGB8I;	}
			break;
		case SINT:{val=GL_R32I;		}
			break;
		case SINT2:{val=GL_RG32I;	}
			break;
		case SINT3:{val=GL_RGB32I;	}
			break;
		case FLOAT:{val=GL_R32F;	}
			break;
		case FLOAT2:{val=GL_RG32F;	}
			break;
		case FLOAT3:{val=GL_RGB32F;	}
			break;
		}
	}*/
	glBindImageTexture(0, tex_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
	MYGLERRORMACRO
}


void Texture::setPixel(glm::ivec2 cr, float d){
	float f[2] = {d, d};
	glTextureSubImage2D(tex_id, 0, cr.x, cr.y, 1, 1, GL_RG, GL_FLOAT, &f[0]);
	MYGLERRORMACRO
}

