#ifndef STDLIB_H
#define STDLIB_H

#include "stddef.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t num, size_t size);

#endif // STDLIB_H

