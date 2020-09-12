#ifndef VIRTUALMATH_MEM_H
#define VIRTUALMATH_MEM_H
#include <string.h>
#include <__macro.h>

extern jmp_buf memVirtualMath_Env;
extern bool memVirtualMathUseJmp;

void *memCalloc(size_t num, size_t size);
void *memRealloc(void *old, size_t size);
char *memStrcpy(const char *str);
char *memStrCharcpy(char *str, size_t nsize, bool free_old, bool write, ...);
char *memStrcatIter(char *base, bool free_base, ...);
char *memStrcat(char *first, char *second, bool free_first, bool free_last);
char *memStrcpySelf(char *str, long times);
char *memStrrev(const char *str);

#define memFree(p) ((p)=((p) != NULL ? (free(p), NULL) : NULL))
#define eqString(str1, str2) (!strcmp(str1, str2))
#define memString(size) (char *)memCalloc(size + 1, sizeof(char))
#define memStrlen(p) (((p) == NULL) ? 0 :strlen(p))

#endif //VIRTUALMATH_MEM_H
