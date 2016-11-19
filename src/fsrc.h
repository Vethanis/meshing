#ifndef FSRC_H
#define FSRC_H

const char* FRAGSRC = "\
#version 430    \n\
smooth in vec3 fragColor;    \n\
flat in int valid;    \n\
layout(std140, binding = 0) uniform UniBlock		\n\
{		\n\
	mat4 MVP;		\n\
	vec4 eye;		\n\
	vec4 light_pos;		\n\
    ivec4 seed;     \n\
};		\n\
out vec4 out_color;    \n\
float randUni(inout uint f){    \n\
    f = (f ^ 61) ^ (f >> 16);    \n\
    f *= 9;    \n\
    f = f ^ (f >> 4);    \n\
    f *= 0x27d4eb2d;    \n\
    f = f ^ (f >> 15);    \n\
    return fract(float(f) * 2.3283064e-10);    \n\
}    \n\
void main(){    \n\
    if(valid == 0)    \n\
        discard;    \n\
    uint s = uint(dot(gl_FragCoord.xy, gl_PointCoord.xy) * seed.x);    \n\
    vec2 coord = gl_PointCoord * 2.0 - 1.0;    \n\
    float dis = length(coord);    \n\
    if(dis > randUni(s)){    \n\
        discard;    \n\
    }    \n\
    out_color = vec4(fragColor, 1.0);    \n\
}    \n\
";

#endif
