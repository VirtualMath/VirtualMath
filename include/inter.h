#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Result;

struct Inter{
    struct Value *base;
    struct LinkValue *link_base;
    struct HashTable *hash_base;
    struct Var *base_var;

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
typedef struct Statement Statement;

Inter *makeInter(char *debug);
void freeInter(Inter *inter, bool show_gc);
void setBaseInterData(struct Inter *inter);
int runCodeBlock(char *code_file, Inter *inter);
void runParser(char *code_file, Inter *inter, Statement **st);
void runCode(Statement *st, Inter *inter);
void mergeInter(Inter *new, Inter *base);
#endif //VIRTUALMATH_INTER_H
