#include "loadfile.h"

#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "assert.h"

char* load_file(const char* path){
    FILE* f = fopen(path, "rb");
    assert(f);
    fseek(f, 0, SEEK_END);
    const size_t sz = size_t(ftell(f));
    char* out = new char[sz + 1];
    rewind(f);
    size_t blocksz = 1, shifts = 0;
    while((blocksz & sz) == 0){
        blocksz = blocksz << 1;
        shifts++;
    }
    fread(out, blocksz, sz >> shifts, f);
    fclose(f);
    out[sz] = 0;
    return out;
}
