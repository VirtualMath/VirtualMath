#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

typedef struct globalInterpreter{
    struct VirtualMathValue *base;
    struct VirtualMathLinkValue *link_base;
    struct VirtualMathHashTable *hash_base;
    struct Statement *statement;
    struct VirtualMathVarList *var_list;
    char *log_dir;  // 记录log文件夹的位置
    FILE *debug;
} Inter;

Inter *makeInter(char *debug);
void freeInter(Inter *inter, bool self);

#endif //VIRTUALMATH_INTER_H
