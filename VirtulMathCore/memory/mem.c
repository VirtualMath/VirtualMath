#include "__virtualmath.h"

jmp_buf memVirtualMath_Env;
bool memVirtualMathUseJmp;

void *memCalloc(size_t num, size_t size){
    void *tmp = calloc(num, size);
    if (tmp == NULL) {
        if (memVirtualMathUseJmp)
            longjmp(memVirtualMath_Env, -1);
        else
            exit(2);
    }
    return tmp;
}

void *memRealloc(void *old, size_t size){
    void *tmp = realloc(old, size);
    if (tmp == NULL) {
        if (memVirtualMathUseJmp)
            longjmp(memVirtualMath_Env, -1);
        else
            exit(2);
    }
    return tmp;
}

char *memStrcpy(const char *str){
    char *tmp = memString(memStrlen(str));
    if (str != NULL)
        strcpy(tmp, str);
    return tmp;
}

wchar_t *memWidecpy(const wchar_t *str){
    wchar_t *tmp = memWide(memWidelen(str));
    if (str != NULL)
        wcscpy(tmp, str);
    return tmp;
}

wchar_t *memWideCharcpy(wchar_t *str, size_t nsize, bool free_old, bool write, ...) {  // 复制str到新的空间，nszie是要扩展的大小。该函数支持让str=NULL，则变为单纯的memString
    size_t base_len = memWidelen(str);
    wchar_t *tmp = memWide(base_len + nsize);
    if (base_len != 0)
        wcscpy(tmp, str);
    if (write){
        va_list argp;
        va_start(argp, write);
        for (int i = 0; i < nsize; i++)
            tmp[base_len + i] = (wchar_t)va_arg(argp, int);
        va_end(argp);
    }
    if (free_old)
        memFree(str);
    return tmp;
}

wchar_t *memWideExpansion(wchar_t *str, size_t nsize, bool free_old) {
    size_t base_len = memWidelen(str);
    wchar_t *tmp = memWide(base_len + nsize);
    if (base_len != 0)
        wcscpy(tmp, str);
    if (free_old)
        memFree(str);
    return tmp;
}

char *memStrcatIter(char *base, bool free_base, ...) {
    va_list ap;
    va_start(ap, free_base);
    for (char *ch = va_arg(ap, char *); ch != NULL; ch = va_arg(ap, char *)) {
        base = memStrcat(base, ch, free_base, false);
        free_base = true;
    }
    va_end(ap);
    return base;
}

char *memStrcat(char *first, char *second, bool free_first, bool free_last) {
    if (first == NULL && second == NULL)
        return NULL;
    else if (first == NULL){
        first = second;
        second = NULL;
        free_first = free_last;
        free_last = false;
    }

    char *new = memString(memStrlen(first) + memStrlen(second));
    strcat(new, first);
    if (second != NULL)
        strcat(new, second);

    if (free_first)
        memFree(first);
    if (free_last)
        memFree(second);
    return new;
}

wchar_t *memWidecat(wchar_t *first, wchar_t *second, bool free_first, bool free_last) {
    if (first == NULL && second == NULL)
        return NULL;
    else if (first == NULL){
        first = second;
        second = NULL;
        free_first = free_last;
        free_last = false;
    }

    wchar_t *new = memWideExpansion(first, memWidelen(second), false);
    if (second != NULL)
        wcscat(new, second);

    if (free_first)
        memFree(first);
    if (free_last)
        memFree(second);
    return new;
}

wchar_t *memWidecpySelf(wchar_t *str, long times){
    bool need_free = false;
    wchar_t *new_str = NULL;
    if (times < 0){
        str = memWiderev(str);
        times = -times;
        need_free = true;
    }
    for (long i=0; i < times; i++)
        new_str = memWidecat(new_str, str, true, false);
    if (need_free)
        memFree(str);
    return new_str;
}

wchar_t *memWiderev(wchar_t *str){
    size_t len_str = memWidelen(str);
    wchar_t *new_str = memWide(len_str);
    for (int i = 0;i < len_str;i++)
        new_str[i] = str[len_str - i - 1];
    return new_str;
}

wchar_t *memStrToWcs(char *str, bool free_old) {
    size_t len = memStrlen(str);
    wchar_t *tmp = memWide(len);
    mbstowcs(tmp, str, len);
    if (free_old)
        memFree(str);
    return tmp;
}

char *memWcsToStr(wchar_t *wcs, bool free_old) {
    size_t len = memWidelen(wcs);
    char *tmp = memString(len);
    wcstombs(tmp, wcs, len);
    if (free_old)
        memFree(wcs);
    return tmp;
}
