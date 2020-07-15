#ifndef VIRTUALMATH_MEM_H
#define VIRTUALMATH_MEM_H
#include <string.h>

void *memFreeCore(void *p);
void *memCalloc(size_t num, size_t size);
void *memRealloc(void *old, size_t size);
char *memStrcpy(size_t nsize, int free_old, char *str, int write, ...);
char *memString(size_t size);
size_t memStrlen(char *p);
#define memFree(p) p=memFreeCore(p)

#endif //VIRTUALMATH_MEM_H
