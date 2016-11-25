#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in int id;

layout(std140, binding = 0) uniform UniBlock
{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
    ivec4 seed;
};

smooth out vec3 fragColor;
flat out int valid;

vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	gl_Position = MVP * vec4(position, 1);
	gl_PointSize = clamp(25.0f / gl_Position.w, 1.0f, 500.0f);

    vec3 V = eye.xyz - position;
    if(dot(V, normal) <= 0.0){
        valid = 0;
        return;
    }
    valid = 1;

	vec3 color = hsv2rgb(vec3(fract(double(id) * 0.0000001), 0.8, 0.8));
	vec3 L = light_pos.xyz;
	float D = max(0.0f, dot(L, normal));
	vec3 H = normalize(normalize(V) + L);
	float S = (D > 0.0f) ? pow(max(0.0f, dot(H, normal)), 64.0f) : 0.0f;

	fragColor = vec3(0.01f, 0.01f, 0.01f) * color + (D * color + S * color);
	fragColor = pow(fragColor, vec3(1.0f / 2.2f));
}
