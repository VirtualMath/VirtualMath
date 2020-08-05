#include "__virtualmath.h"

void *memFreeCore(void *p){
    if (p != NULL)
        free(p);
    return NULL;
}

void *memCalloc(size_t num, size_t size){
    if (num == 0 || size == 0)
        return NULL;
    void *tmp = calloc(num, size);
    return tmp;
}

void *memRealloc(void *old, size_t size){
    if (size == 0)
        return NULL;
    void *tmp;
    if (old == NULL)
        tmp = memCalloc(1,size);
    else
        tmp = realloc(old, size);
    return tmp;
}

size_t memStrlen(char *p){  // 可以读取NULL的strlen
    if (p == NULL){
        return 0;
    }
    else{
        return strlen(p);
    }
}

char *memString(size_t size) {  // 比memCalloc多了一个设置\0的步骤
    if (size == 0){
        return NULL;
    }
    char *tmp = (char *)memCalloc(size + 1, sizeof(char));
    tmp[size] = '\0';
    return tmp;
}

char *memStrcpy(char *str, size_t nsize, bool free_old, bool write, ...) {  // 复制str到新的空间，nszie是要扩展的大小。该函数支持让str=NULL，则变为单纯的memString
    char *tmp = memString(memStrlen(str) + nsize + 1);
    if (str != NULL){
        strcpy(tmp, str);
    }
    if (write){
        va_list argp;
        va_start(argp, write);
        for (int i = 0; i < nsize; i++){
            tmp[memStrlen(str) + i] = (char)va_arg(argp, int);
        }
        va_end(argp);
    }
    if (free_old){
        memFree(str);
    }
    return tmp;
}

char *memStrcat(char *first, char *second, bool free_old) {
    if (first == NULL && second == NULL)
        return NULL;
    else if (first == NULL){
        first = second;
        second = NULL;
        free_old = false;
    }

    char *new = memStrcpy(first, memStrlen(second), false, false);
    if (second != NULL){
        strcat(new, second);
    }
    if (free_old)
        memFree(first);
    return new;
}

char *memStrcpySelf(char *str, NUMBER_TYPE times){
    bool need_free = false;
    if (times < 0){
        str = memStrrev(str);
        times = -times;
        need_free = true;
    }
    char *new_str = memStrcpy(str, 0, false, false), *tmp;
    for (NUMBER_TYPE i=0; i < times - 1; i++){
        tmp = memStrcat(new_str, str, false);
        memFree(new_str);
        new_str = tmp;
    }
    if (need_free){
        memFree(str);
    }
    return new_str;
}

char *memStrrev(char *str){
    size_t len_str = memStrlen(str);
    char *new_str = memString(len_str);
    for (int i = 0;i < len_str;i++){
        new_str[i] = str[len_str - i - 1];
    }
    return new_str;
}
