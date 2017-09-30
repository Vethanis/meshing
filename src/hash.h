#pragma once 


inline unsigned fnv(const char* name){
    const unsigned char* data = (const unsigned char*)name;
    unsigned val = 3759247821;
    while(*data){
        val ^= *data++;
        val *= 0x01000193;
    }
    val &= 0x7fffffff;
    val |= val==0;
    return val;
}

inline unsigned fnv(const void* p, const unsigned len){
    const unsigned char* data = (const unsigned char*)p;
    unsigned val = 3759247821;
    for(unsigned i = 0; i < len; i++){
        val ^= data[i];
        val *= 0x01000193;
    }
    val &= 0x7fffffff;
    val |= val==0;
    return val;
}
