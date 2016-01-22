#ifndef TEST_H
#define TEST_H

#include "camera.h"
#include "texture.h"
#include "debugmacro.h"

using namespace glm;

#define MAX_DEPTH 10
#define FAR cam.getFar()
#define NEAR cam.getNear()
#define EYE cam.getEye()

vec2 iadd(vec2 a, vec2 b){return a + b;}
vec2 iadd(vec2 a, float b){return a + b;}
vec2 iadd(float a, vec2 b){return a + b;}
vec2 isub(vec2 a, vec2 b){return a - b.yx();}
vec2 isub(vec2 a, float b){return a - b;}
vec2 isub(float a, vec2 b){return a - b.yx();}
vec2 imul(vec2 a, vec2 b){
	vec4 f = vec4(a.xxyy() * b.xyxy());	
	return vec2(
		min(min(f[0],f[1]),min(f[2],f[3])),
		max(max(f[0],f[1]),max(f[2],f[3])));
}
vec2 imul(float a, vec2 b){
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}
vec2 imul(vec2 b, float a){
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}
vec2 ipow2(vec2 a){	
	return (a.x>=0.0f)?vec2(a*a):(a.y<0.0f)?vec2((a*a).yx()):vec2(0.0f,max(a.x*a.x,a.y*a.y));
}
vec2 ipow4(vec2 a){
	return (a.x>=0.0f)?vec2(a*a*a*a):(a.y<0.0f)?vec2((a*a*a*a).yx()):vec2(0.0f,max(a.x*a.x*a.x*a.x,a.y*a.y*a.y*a.y));
}
vec2 ipow6(vec2 a){
	return (a.x>=0.0f)?vec2(a*a*a*a*a*a):(a.y<0.0f)?vec2((a*a*a*a*a*a).yx()):vec2(0.0f,max(a.x*a.x*a.x*a.x*a.x*a.x,a.y*a.y*a.y*a.y*a.y*a.y));
}
vec2 imin(vec2 a, vec2 b){
	return vec2(min(a.x,b.x),min(a.y,b.y));
}
vec2 imax(vec2 a, vec2 b){
	return vec2(max(a.x,b.x),max(a.y,b.y));
}
vec3 imin(vec3 a, vec3 b){
	return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}
vec3 imax(vec3 a, vec3 b){
	return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}
float imin(vec3 a){
	return min(min(a.x, a.y), a.z);
}
float imax(vec3 a){
	return max(max(a.x, a.y), a.z);
}
vec2 iabs(vec2 a){
  if (a.x >= 0.0f)
      return a;
  if (a.y <= 0.0f) 
      return vec2(-a.y, -a.x);
  return vec2(0, max(-a.x, a.y));
}
vec2 iunion(vec2 a, vec2 b){
	return vec2(min(a.x, b.x), max(a.y, b.y));
}
vec2 iintersect(vec2 a, vec2 b){
	return vec2(max(a.x, b.x), min(a.y, b.y));
}
vec2 ismoothmin(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0f)));
	return isub(imin(a, b), imul(ipow2(e), 0.25f/r));
}
vec2 ismoothmax(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0f)));
	return iadd(imax(a, b), imul(ipow2(e), 0.25f/r));
}
vec4 imod2(vec2 a, float b){
	if ((a.y - a.x) >= b)
       		return vec4(0.0f, b, 0.0f, b);
	else {
            a = mod(a,b);
            if (a.y < a.x)
                return vec4(a.x,b,0.0f,a.y);
                return vec4(a,a);
	}
}
vec2 isqrt(vec2 a){
	return vec2(sqrt(a.x), sqrt(a.y));
}
vec2 ilength(vec2 a, vec2 b){
	return isqrt(ipow2(a) + ipow2(b));
}
vec2 itri(vec2 x, float p){
	vec4 m = imod2(x + (p*0.5f), p) - 0.5f*p;
	return iunion(iabs(m.xy()), iabs(m.zw()));
}
vec2 itorus(vec2 x, vec2 y, vec2 z, vec2 t){
	return isub(ilength(isub(ilength(x, y), t.x), z), t.y);
}
vec2 isphere(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow2(x-c.x) + ipow2(y-c.y) + ipow2(z-c.z) - r*r;
}
vec2 isphere2(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow4(x-c.x) + ipow4(y-c.y) + ipow4(z-c.z) - r*r;
}
vec2 isphere3(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow6(x-c.x) + ipow6(y-c.y) + ipow6(z-c.z) - r*r;
}
vec2 icube(vec2 x, vec2 y, vec2 z, float s){
	return isub(
		imax(
			imax(
				iabs(x), 
				iabs(y)),
			iabs(z)),
		s);
}
vec2 ibox(vec3 l, vec3 h, vec3 c, vec3 d){
	vec3 a = c - d;
	vec3 b = c + d;
	vec3 la = l - a;
	vec3 lb = l - b;
	vec3 ha = h - a;
	vec3 hb = h - b;
	return vec2(imax(imin(la, lb)), imin(imax(ha, hb)));
}
bool contains(vec2 a, float s){
	return a.x <= s && a.y >= s;
}
float width(vec2 a){
	return a.y - a.x;
}
float center(vec2 a){
	return 0.5f*(a.x+a.y);
}
vec2 widen(vec2 t, float f){
	return vec2(t.x - f, t.y + f);
}
vec2 inear(vec2 a){
	return vec2(a.x, center(a));
}
vec2 ifar(vec2 a){
	return vec2(center(a), a.y);
}
vec2 ix(const vec3& a, const vec3& b){
	return vec2(min(a.x, b.x), max(a.x, b.x));
}
vec2 iy(const vec3& a, const vec3& b){
	return vec2(min(a.y, b.y), max(a.y, b.y));
}
vec2 iz(const vec3& a, const vec3& b){
	return vec2(min(a.z, b.z), max(a.z, b.z));
}
vec2 ipop(vec2 a){
	float dd = center(a);
	return vec2(a.x+dd, a.y+2.0f*dd);
}
float maxabs(vec2 a){
	return max(abs(a.x), abs(a.y));
}

vec2 paniq_scene(vec2 a, vec2 b, vec2 c){
	vec2 d = itri(a, 40.0f);
	vec2 e = itri(b, 40.0f);
	vec2 f = itri(c, 40.0f);
	return imin(
		itorus(d, e, f, vec2(1.0f, 0.2f)),
		icube(d, e, f, 0.5f)
		);
}

float toExp(Camera& cam, float z){
	return (1.f/z - 1.f/NEAR) / (1.f/FAR - 1.f/NEAR);
}
float toLin(Camera& cam, float f){
	return 1.0f / (f * (1.f/FAR - 1.f/NEAR) + (1.f/NEAR));
}

vec3 toWorld(Camera& cam, const vec3& a){
	vec4 t(a, 1.0f);
	t = cam.getIV() * t;
	return vec3(t);
}

void toInterval(Camera& cam, vec2 u, vec2 v, vec2 t, vec3& l, vec3& h){
	float fx = t.y*sin(cam.getFov()*0.5f);
	l = toWorld(cam, vec3(u.x*fx, v.x*fx, -t.x));
	h = toWorld(cam, vec3(u.y*fx, v.y*fx, -t.y));
}

vec2 map(Camera& cam, vec2 u, vec2 v, vec2 t){
	vec3 a, b;
	printf("UVT: \n");print(u); print(v); print(t);
	toInterval(cam, u, v, t, a, b);
	vec2 c = ix(a, b); vec2 d = iy(a, b); vec2 e = iz(a, b);
	printf("xyz: \n");print(c); print(d); print(e);
	return paniq_scene(c, d, e);
	//return isphere(c, d, e, vec3(0.f), 1.f);
}

vec2 strace(Camera& cam, vec2 u, vec2 v, vec2 t, float e){
	const int sz = 16;
	vec2 stack[sz];
	int end = 0;
	stack[end] = t;
	int entries = 1;
	for(int i = 0; i < 300; i++){
		vec2 cur = stack[end];
		end--; if (end < 0) end = sz-1;
		entries--;
		vec2 F = map(cam, u, v, cur);
		if(contains(F, 0.0f)){
			if(width(cur) < e*center(cur))return cur;
			end = (end+1)%sz;
			stack[end] = ifar(cur);
			end = (end+1)%sz;
			stack[end] = inear(cur);
			entries = min(entries+2, sz);
			continue;
		}
		if(entries <= 0) break;
	}
	return vec2(cam.getFar());
}

void getUVs(vec2& u, vec2& v, ivec2 cr, int depth){
	int dim = (depth == 0) ? 1 : int(pow(2, depth));
	cr = cr * dim / 1024;
	float dif = 2.0f / dim;
	u.x = -1.0f + cr.x*dif;
	u.y = -1.0f + cr.x*dif + dif;
	v.x = -1.0f + cr.y*dif;
	v.y = -1.0f + cr.y*dif + dif;
}

vec2 subdivide(Camera& cam, vec2 t, ivec2 cr, float e){
	vec2 u, v;
	for(int j = 0; j < MAX_DEPTH; j++){
		t.y = cam.getFar();
		getUVs(u, v, cr, j);
		t = strace(cam, u, v, t, e);
		if(t.y >= cam.getFar()) return vec2(cam.getFar());
		e = e * 0.5f;
	}
	return t;
}

void test(Camera& cam, Texture& dbuf, ivec2 WH){
	for(int i = 0; i < WH.x*WH.y; i++){
		ivec2 pix = ivec2(i%WH.x, i/WH.x);
		vec2 F = subdivide(cam, vec2(NEAR, FAR), pix, 0.5f);
		if(F.y >= FAR) continue;
		float f = center(F);
		dbuf.setPixel(pix, f);
	}
}

#endif
