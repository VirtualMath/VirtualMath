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

char *memStrcpy(const char *const str){
    char *tmp = memString(memStrlen(str));
    if (str != NULL)
        strcpy(tmp, str);
    return tmp;
}

char *memStrCharcpy(char *str, size_t nsize, bool free_old, bool write, ...) {  // 复制str到新的空间，nszie是要扩展的大小。该函数支持让str=NULL，则变为单纯的memString
    char *tmp = memString(memStrlen(str) + nsize);
    size_t base_len = memStrlen(str);
    if (base_len != 0)
        strcpy(tmp, str);
    if (write){
        va_list argp;
        va_start(argp, write);
        for (int i = 0; i < nsize; i++)
            tmp[base_len + i] = (char)va_arg(argp, int);
        va_end(argp);
    }
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

    char *new = memStrCharcpy(first, memStrlen(second), false, false);
    if (second != NULL)
        strcat(new, second);

    if (free_first)
        memFree(first);
    if (free_last)
        memFree(second);
    return new;
}

char *memStrcpySelf(char *str, long times){
    bool need_free = false;
    char *new_str = NULL;
    if (times < 0){
        str = memStrrev(str);
        times = -times;
        need_free = true;
    }
    for (long i=0; i < times; i++)
        new_str = memStrcat(new_str, str, true, false);
    if (need_free)
        memFree(str);
    return new_str;
}

char *memStrrev(const char *const str){
    size_t len_str = memStrlen(str);
    char *new_str = memString(len_str);
    for (int i = 0;i < len_str;i++)
        new_str[i] = str[len_str - i - 1];
    return new_str;
}
