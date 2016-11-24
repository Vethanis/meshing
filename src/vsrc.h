#ifndef VSRC_H
#define VSRC_H

const char* VERTSRC =
"#version 430 core		\n\
layout(location = 0) in vec3 position;		\n\
layout(location = 1) in vec3 normal;		\n\
layout(location = 2) in int id;		\n\
layout(std140, binding = 0) uniform UniBlock		\n\
{		\n\
	mat4 MVP;		\n\
	vec4 eye;		\n\
	vec4 light_pos;		\n\
    ivec4 seed;     \n\
};		\n\
smooth out vec3 fragColor;		\n\
flat out int valid;    \n\
void main() {		\n\
	gl_Position = MVP * vec4(position, 1);		\n\
	gl_PointSize = clamp(50.0f / gl_Position.w, 1.0f, 100.0f);		\n\
        vec3 V = eye.xyz - position;		\n\
        if(dot(V, normal) <= 0.0){		\n\
            valid = 0;		\n\
            return;		\n\
        }		\n\
        valid = 1;		\n\
	vec3 color = normal * 0.5f + 0.5f;		\n\
	vec3 L = normalize(vec3(0.5f, 0.5f, 0.5f));		\n\
	float D = max(0.0f, dot(L, normal));		\n\
	vec3 H = normalize(normalize(V) + L);		\n\
	float S = (D > 0.0f) ? pow(max(0.0f, dot(H, normal)), 64.0f) : 0.0f;		\n\
	fragColor = vec3(0.01f, 0.01f, 0.01f) + (D * color + S * color);		\n\
	fragColor = pow(fragColor, vec3(1.0f / 2.2f));		\n\
}		\n\
";
#endif
