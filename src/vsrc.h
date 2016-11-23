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
vec3 hsv2rgb(vec3 c) {		\n\
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);		\n\
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);		\n\
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);		\n\
}		\n\
vec3 id_to_color() {		\n\
	float hue = sin(float(id) * 0.1) * 3.141592;		\n\
	float sat = 0.9;		\n\
	float val = 0.9;		\n\
	return hsv2rgb(vec3(hue, sat, val));		\n\
}		\n\
void main() {		\n\
	gl_Position = MVP * vec4(position, 1);		\n\
	gl_PointSize = clamp(45.0f / gl_Position.w, 1.0f, 100.0f);		\n\
        vec3 V = eye.xyz - position;		\n\
        if(dot(V, normal) <= 0.0){		\n\
            valid = 0;		\n\
            return;		\n\
        }		\n\
        valid = 1;		\n\
	vec3 color = normal * 0.5f + 0.5f;		\n\
	vec3 L = normalize(light_pos.xyz - position);		\n\
	float D = max(0.0f, dot(L, normal));		\n\
	vec3 H = normalize(normalize(V) + L);		\n\
	float S = (D > 0.0f) ? pow(max(0.0f, dot(H, normal)), 64.0f) : 0.0f;		\n\
	float dist = max(1.0f, gl_Position.w * gl_Position.w);		\n\
	fragColor = vec3(0.001f, 0.0005f, 0.0005f) + (D * color + S * color) / dist;		\n\
	fragColor = pow(fragColor, vec3(1.0f / 2.2f));		\n\
}		\n\
";
#endif
