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
	vec4 mat_params;
};

smooth out vec3 fragColor;
flat out int valid;

// -------------------------------------------------------------------

float rand( inout uint f) {
    f = (f ^ 61u) ^ (f >> 16u);
    f *= 9u;
    f = f ^ (f >> 4u);
    f *= 0x27d4eb2d;
    f = f ^ (f >> 15u);
    return fract(float(f) * 2.3283064e-10);
}

float randBi(inout uint s){
    return rand(s) * 2.0 - 1.0;
}

// -------------------------------------------------------------------

float DisGGX(vec3 N, vec3 H, float roughness){
    float a = roughness * roughness;
    float a2 = a * a;
    float NdH = max(dot(N, H), 0.0);
    float NdH2 = NdH * NdH;

    float nom = a2;
    float denom_term = (NdH2 * (a2 - 1.0) + 1.0);
    float denom = 3.141592 * denom_term * denom_term;

    return nom / denom;
}

float GeomSchlickGGX(float NdV, float roughness){
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdV;
    float denom = NdV * (1.0 - k) + k;

    return nom / denom;
}

float GeomSmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdV = max(dot(N, V), 0.0);
    float NdL = max(dot(N, L), 0.0);
    float ggx2 = GeomSchlickGGX(NdV, roughness);
    float ggx1 = GeomSchlickGGX(NdL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ------------------------------------------------------------------------

vec3 pbr_lighting(vec3 V, vec3 L, vec3 N, vec3 albedo, vec3 radiance, float metalness, float roughness){
    float NdL = max(0.0, dot(N, L));
    vec3 F0 = mix(vec3(0.04), albedo, metalness);
    vec3 H = normalize(V + L);

    float NDF = DisGGX(N, H, roughness);
    float G = GeomSmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nom = NDF * G * F;
    float denom = 4.0 * max(dot(N, V), 0.0) * NdL + 0.001;
    vec3 specular = nom / denom;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metalness);

    return (kD * albedo / 3.141592 + specular) * radiance * NdL;
}

// --------------------------------------------------------------------------

void main() {
	gl_Position = MVP * vec4(position, 1);
	gl_PointSize = clamp(70.0f / gl_Position.w, 1.0f, 500.0f);

    vec3 V = normalize(eye.xyz - position);
    if(dot(V, normal) <= 0.0){
        valid = 0;
        return;
    }
    valid = 1;

	vec3 L = light_pos.xyz;
	vec3 lighting = pbr_lighting(V, L, normal, color, vec3(1.0), mat_params.x, mat_params.y);

	uint s = uint(seed.x) 
        ^ uint(gl_Position.x * 10.0) 
        ^ uint(gl_Position.y * 1000.0);

	lighting.rgb = lighting.rgb / (lighting.rgb + vec3(1.0));
	lighting.rgb = pow(lighting.rgb, vec3(1.0 / 2.2));

    fragColor = lighting.rgb;
}
