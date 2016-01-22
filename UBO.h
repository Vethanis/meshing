#ifndef UBO_H
#define UBO_H

class UBO{
	unsigned id;
public:
	UBO(void* ptr, size_t size, unsigned binding);
	~UBO();
	void upload(void* ptr, size_t size);
};

#endif
