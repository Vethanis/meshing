#version 430

smooth in vec3 fragColor;
flat in int valid;

layout(std140, binding = 0) uniform UniBlock
{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
        ivec4 seed;
};

out vec4 out_color;

float randUni(inout uint f){
    f = (f ^ 61) ^ (f >> 16);
    f *= 9;
    f = f ^ (f >> 4);
    f *= 0x27d4eb2d;
    f = f ^ (f >> 15);
    return fract(float(f) * 2.3283064e-10);
}

void main(){
    if(valid == 0)
        discard;

    uint s = uint(dot(gl_FragCoord.xy, gl_PointCoord.xy) * seed.x);
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    
    if(length(coord) > randUni(s)){
        discard;
    }

    out_color = vec4(fragColor, 1.0);
}
