#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Result;

struct Inter{
    struct Value *base;
    struct LinkValue *link_base;
    struct HashTable *hash_base;
    struct Var *base_var;
    struct LinkValue *base_father;

    struct VarList *var_list;
    struct InterData{
        FILE *inter_stdout;
        FILE *inter_stderr;
        bool is_stdout;
        bool is_stderr;

        struct Value *object;
        struct Value *vobject;
        struct Value *num;
        struct Value *str;
        struct Value *bool_;
        struct Value *pass_;
        struct Value *list;
        struct Value *dict;
        struct Value *function;
        struct Value *none;
        struct Value *list_iter;
        struct Value *dict_iter;

        char *var_str_prefix;
        char *var_num_prefix;
        char *var_bool_prefix;
        char *var_none;
        char *var_pass;
        char *var_class_prefix;
        char *var_object_prefix;
        char *object_init;
        char *object_new;
        char *object_call;
        char *object_enter;
        char *object_exit;
        char *object_add;
        char *object_sub;
        char *object_mul;
        char *object_div;
        char *object_del;
        char *object_down;
        char *object_slice;
        char *object_iter;
        char *object_next;
        int default_pt_type;
    } data;
};

typedef struct Inter Inter;
typedef struct Statement Statement;
typedef enum ResultType ResultType;

Inter *makeInter(char *out, char *error_, struct LinkValue *belong);
void freeInter(Inter *inter, bool show_gc);
void setBaseInterData(struct Inter *inter);
ResultType runCodeBlock(char *code_file, Inter *inter);
ResultType runCodeStdin(Inter *inter);
void runParser(char *code_file, Inter *inter, bool is_one, Statement **st);
ResultType runCode(Statement *st, Inter *inter);
void mergeInter(Inter *new, Inter *base);
#endif //VIRTUALMATH_INTER_H
