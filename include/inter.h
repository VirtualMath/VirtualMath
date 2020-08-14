#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Result;

struct Inter{
    struct Value *base;
    struct LinkValue *link_base;
    struct HashTable *hash_base;
    struct Var *base_var;

    struct Statement *statement;
    struct VarList *var_list;
    struct InterData{
        FILE *debug;
        FILE *error;
        char *log_dir;  // 记录log文件夹的位置
        char *var_str_prefix;
        char *var_num_prefix;
        char *var_defualt;
        char *object_init;
    } data;
};

typedef struct Inter Inter;

Inter *makeInter(char *code_file, char *debug);
void freeInter(Inter *inter, bool self);
void setBaseInterData(struct Inter *inter);
Inter *newInter(char *code_file, char *debug_dir,struct Result *global_result, int *status);
Inter *runBaseInter(char *code_file, char *debug_dir, int *status);
#endif //VIRTUALMATH_INTER_H
