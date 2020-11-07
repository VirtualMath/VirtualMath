#ifndef VIRTUALMATH_MEM_H
#define VIRTUALMATH_MEM_H
#include <__macro.h>
#include <string.h>

extern jmp_buf memVirtualMath_Env;
extern bool memVirtualMathUseJmp;

void memError(void);
void *memCalloc(size_t num, size_t size);
void *memRealloc(void *old, size_t size);
char *memStrcpy(const char *str);
wchar_t *memWidecpy(const wchar_t *str);
wchar_t *memWideCharcpy(wchar_t *str, size_t nsize, bool free_old, bool write,
                        ...);
wchar_t *memWideExpansion(wchar_t *str, size_t nsize, bool free_old);
char *memStrcatIter(char *base, bool free_base, ...);
char *memStrcat(char *first, char *second, bool free_first, bool free_last);
wchar_t *memWidecat(wchar_t *first, wchar_t *second, bool free_first,
                    bool free_last);
wchar_t *memWidecpySelf(wchar_t *str, long times);
wchar_t *memWiderev(wchar_t *str);
char *memWcsToStr(wchar_t *wcs, bool free_old);
wchar_t *memStrToWcs(char *str, bool free_old);

#define memFree(p) ((p) = ((p) != NULL ? (free((p)), NULL) : NULL))
#define eqString(str1, str2) (!strcmp((str1), (str2)))
#define eqWide(wid1, wid2) (!wcscmp((wchar_t *)(wid1), (wchar_t *)(wid2)))
#define memString(size) (char *)memCalloc((size) + 1, sizeof(char))
#define memWide(size) (wchar_t *)memCalloc((size) + 1, sizeof(wchar_t))
#define memStrlen(p) (((p) == NULL) ? 0 : strlen((p)))
#define memWidelen(p) (((p) == NULL) ? 0 : wcslen((p)))

#define MACRO_CALLOC(var, n, size)                                             \
  ((((var) = (typeof(var))calloc(n, size)) == NULL) ? (memError()) : (var))

#endif // VIRTUALMATH_MEM_H
