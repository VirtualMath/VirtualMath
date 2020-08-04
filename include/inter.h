#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Inter{
    struct Value *base;
    struct LinkValue *link_base;
    struct HashTable *hash_base;
    struct Statement *statement;
    struct VarList *var_list;
    struct InterData{
        FILE *debug;
        char *log_dir;  // 记录log文件夹的位置
        char *var_str_prefix;
        char *var_num_prefix;
        char *var_defualt;
    } data;
};

typedef struct Inter Inter;

Inter *makeInter(char *debug);
void freeInter(Inter *inter, bool self);
void setBaseInterData(struct Inter *inter);

#endif //VIRTUALMATH_INTER_H
