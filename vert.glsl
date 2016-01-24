#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in float radius;

layout(std140, binding=0) uniform UniBlock
{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
};

smooth out vec3 fragColor;

#define COLOR

void main(){
	gl_Position = MVP * vec4(position, 1);
	gl_PointSize = clamp(radius / gl_Position.w, 5.0f, 500.0f);
#ifdef COLOR
	if(light_pos.w == 1.0f){
		vec3 L = normalize(light_pos.xyz - position);
		float D = max(0.0f, dot(L, normal));
		vec3 H = normalize(normalize(eye.xyz - position) + L);
		float S = (D > 0.0f) ? pow(max(0.0f, dot(H, normal)), 16.0f) : 0.0f;
		float dist2 = dot(light_pos.xyz - position, light_pos.xyz - position);
		fragColor = vec3(0.0001f, 0.00005f, 0.00005f) + (D * color + S * color)*30.0f / dist2;
	}
	else
		fragColor = color;
	fragColor = pow(fragColor, vec3(1.0f / 2.2f));
#endif
#ifdef NORMAL
	fragColor = normal;
#endif
}
