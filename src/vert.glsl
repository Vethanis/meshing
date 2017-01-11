#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

layout(std140) uniform UniBlock
{
	mat4 MVP;
	vec4 eye;
	vec4 light_pos;
    ivec4 seed;
};

smooth out vec3 fragColor;
flat out int valid;

void main() {
	gl_Position = MVP * vec4(position, 1);
	gl_PointSize = clamp(25.0f / gl_Position.w, 1.0f, 500.0f);

    vec3 V = eye.xyz - position;
    if(dot(V, normal) <= 0.0){
        valid = 0;
        return;
    }
    valid = 1;

	vec3 L = light_pos.xyz;
	float D = max(0.0f, dot(L, normal));
	vec3 H = normalize(normalize(V) + L);
	float S = (D > 0.0f) ? pow(max(0.0f, dot(H, normal)), 64.0f) : 0.0f;

	fragColor = vec3(0.001f, 0.001f, 0.001f) * color + (D * color + S * color);
	fragColor = pow(fragColor, vec3(1.0f / 2.2f));
}
