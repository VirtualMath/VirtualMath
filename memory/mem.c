#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

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
    char *tmp = (char *)memCalloc(size + 1, sizeof(char));
    tmp[size] = '\0';
    return tmp;
}

char *memStrcpy(size_t nsize, int free_old, char *str, int write, ...) {  // 复制str到新的空间，nszie是要扩展的大小。该函数支持让str=NULL，则变为单纯的memString
    char *tmp = memString(memStrlen(str) + nsize + 1);
    if (str != NULL){
        strcpy(tmp, str);
        tmp[memStrlen(str)] = (char)0;  // 去除多余的\0
        if (free_old){
            memFree(str);
        }
    }
    if (write){
        va_list argp;
        va_start(argp, write);
        for (int i = 0; i < nsize; i++){
            tmp[memStrlen(str) + i] = (char)va_arg(argp, int);
        }
        va_end(argp);
    }
    return tmp;
}
