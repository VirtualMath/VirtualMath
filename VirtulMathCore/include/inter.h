#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Result;

struct Inter{
    struct Value *base;
    struct LinkValue *link_base;
    struct HashTable *hash_base;
    struct Var *base_var;
    struct LinkValue *base_belong;
    struct Package *package;

    struct VarList *var_list;
    struct InterData{
        FILE *inter_stdout;
        FILE *inter_stderr;
        FILE *inter_stdin;
        bool is_stdout;
        bool is_stderr;
        bool is_stdin;

        struct LinkValue *object;
        struct LinkValue *vobject;
        struct LinkValue *int_;
        struct LinkValue *dou;
        struct LinkValue *str;
        struct LinkValue *bool_;
        struct LinkValue *pass_;
        struct LinkValue *lib_;
        struct LinkValue *file;
        struct LinkValue *tuple;
        struct LinkValue *list;
        struct LinkValue *dict;
        struct LinkValue *function;
        struct LinkValue *none;
        struct LinkValue *list_iter;
        struct LinkValue *dict_iter;
        struct LinkValue *pointer;

        struct LinkValue *base_exc;
        struct LinkValue *sys_exc;
        struct LinkValue *keyInterrupt_exc;
        struct LinkValue *quit_exc;
        struct LinkValue *exc;
        struct LinkValue *type_exc;
        struct LinkValue *arg_exc;
        struct LinkValue *per_exc;
        struct LinkValue *name_exc;
        struct LinkValue *goto_exc;
        struct LinkValue *result_exc;
        struct LinkValue *assert_exc;

        struct LinkValue *key_exc;
        struct LinkValue *index_exc;
        struct LinkValue *stride_exc;
        struct LinkValue *super_exc;
        struct LinkValue *iterstop_exc;
        struct LinkValue *import_exc;
        struct LinkValue *include_exp;

        wchar_t *var_str_prefix;
        wchar_t *var_int_prefix;
        wchar_t *var_bool_prefix;
        wchar_t *var_file_prefix;
        wchar_t *var_none;
        wchar_t *var_pass;
        wchar_t *var_class_prefix;
        wchar_t *var_object_prefix;
        wchar_t *object_init;
        wchar_t *object_new;
        wchar_t *object_call;
        wchar_t *object_enter;
        wchar_t *object_exit;
        wchar_t *object_add;
        wchar_t *object_sub;
        wchar_t *object_mul;
        wchar_t *object_div;
        wchar_t *object_del;
        wchar_t *object_down;
        wchar_t *object_slice;
        wchar_t *object_iter;
        wchar_t *object_next;
        wchar_t *object_repo;
        wchar_t *object_bool;
        wchar_t *object_name;
        wchar_t *object_self;
        wchar_t *object_father;
        wchar_t *object_message;
        wchar_t *object_str;
        wchar_t *object_down_assignment;
        wchar_t *object_slice_assignment;
        wchar_t *object_down_del;
        wchar_t *object_slice_del;
        wchar_t *object_attr;

        int default_pt_type;
    } data;
};

typedef struct Inter Inter;
typedef struct Statement Statement;
typedef enum ResultType ResultType;

Inter *makeInter(char *out, char *error_, char *in, LinkValue *belong);
void freeInter(Inter *inter, bool show_gc);
void setBaseInterData(struct Inter *inter);
void runCodeStdin(Inter *inter, char *hello_string);
void runCodeFile(Inter *inter, char *file[]);
bool runParser(char *code_file, Inter *inter, bool is_one, Statement **st);
void mergeInter(Inter *new, Inter *base);
Inter *deriveInter(LinkValue *belong, Inter *inter);
#endif //VIRTUALMATH_INTER_H
