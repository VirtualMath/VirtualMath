#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

typedef struct Inter{
    struct Value *base;
    struct LinkValue *link_base;
    struct HashTable *hash_base;
    struct Statement *statement;
    struct VarList *var_list;
    char *log_dir;  // 记录log文件夹的位置
    FILE *debug;
} Inter;

Inter *makeInter(char *debug);
void freeInter(Inter *inter, bool self);

#endif //VIRTUALMATH_INTER_H
