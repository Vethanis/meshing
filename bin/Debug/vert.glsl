#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in int id;

layout(std140, binding=0) uniform UniBlock
{
    mat4 MVP;
    vec4 eye;
    vec4 light_pos;
};

smooth out vec3 fragColor;

vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 id_to_color(){
    float hue = sin(float(id) * 0.01) * 360.0;
    float sat = 0.9;
    float val = 0.9;
    return hsv2rgb(vec3(hue, sat, val));
}

void main(){
    gl_Position = MVP * vec4(position, 1);
    gl_PointSize = clamp(50.0f / gl_Position.w, 5.0f, 500.0f);

    vec3 color = id_to_color();
    vec3 L = normalize(light_pos.xyz - position);
    float D = max(0.0f, dot(L, normal));
    vec3 H = normalize(normalize(eye.xyz - position) + L);
    float S = (D > 0.0f) ? pow(max(0.0f, dot(H, normal)), 16.0f) : 0.0f;
    float dist = max(1.0f, gl_Position.w * gl_Position.w);
    fragColor = vec3(0.001f, 0.0005f, 0.0005f) + (D * color + S * color) / dist;
    fragColor = pow(fragColor, vec3(1.0f / 2.2f));

}
