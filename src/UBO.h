#ifndef UBO_H
#define UBO_H

class UBO{
    unsigned id;
public:
    UBO(void* ptr, size_t size, const char* name, unsigned* programs, int num_progs);
    ~UBO();
    void upload(void* ptr, size_t size);
};

#endif
