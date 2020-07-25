#ifndef VIRTUALMATH_MEM_H
#define VIRTUALMATH_MEM_H
#include <string.h>
#include <__macro.h>

void *memFreeCore(void *p);
void *memCalloc(size_t num, size_t size);
void *memRealloc(void *old, size_t size);
char *memStrcpy(char *str, size_t nsize, int free_old, int write, ...);
char *memString(size_t size);
size_t memStrlen(char *p);
char *memStrcat(char *first, char *second);
char *memStrcpySelf(char *str, number_type times);
char *memStrrev(char *str);

#define memFree(p) p=memFreeCore(p)
#define eqString(str1, str2) (!strcmp(str1, str2))

#endif //VIRTUALMATH_MEM_H
