#version 330

smooth in vec3 fragColor;
flat in int valid;

layout(std140) uniform UniBlock
{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
    ivec4 seed;
};

out vec4 out_color;

float randUni(inout int sd){
    sd = (sd ^ 61) ^ (sd >> 16);
    sd *= 9;
    sd = sd ^ (sd >> 4);
    sd *= 0x27d4eb2d;
    sd = sd ^ (sd >> 15);
    return fract(float(sd) * 2.3283064e-10);
}

void main(){
    if(valid == 0)
        discard;

    int s = int(dot(gl_FragCoord.xy, gl_PointCoord.xy) * float(seed.x));
    vec2 coord = gl_PointCoord * 2.0 - 1.0;

    if(length(coord) > randUni(s)){
        discard;
    }

    out_color = vec4(fragColor, 1.0);
}
