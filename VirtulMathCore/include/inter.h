#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Result;

#define BASEOBJSZIE (17)
#define VARNAMESIZE (8)
#define BASEEXCESIZE (19)
#define MAGFUNCSIZE (27)

#define B_OBJECT (0)
#define B_VOBJECT (1)
#define B_INT_ (2)
#define B_DOU (3)
#define B_STR (4)
#define B_BOOL (5)
#define B_PASS (6)
#define B_LIB (7)
#define B_FILE (8)
#define B_TUPLE (9)
#define B_LIST (10)
#define B_DICT (11)
#define B_FUNCTION (12)
#define B_NONE (13)
#define B_LISTITER (14)
#define B_DICTITER (15)
#define B_POINTER (16)

#define VN_str (0)
#define VN_num (1)
#define VN_bool (2)
#define VN_file (3)
#define VN_none (4)
#define VN_pass (5)
#define VN_class (6)
#define VN_obj (7)

#define M_INIT (0)
#define M_NEW (1)
#define M_CALL (2)
#define M_ENTER (3)
#define M_EXIT (4)
#define M_ADD (5)
#define M_SUB (6)
#define M_MUL (7)
#define M_DIV (8)
#define M_DEL (9)
#define M_DOWN (10)
#define M_SLICE (11)
#define M_ITER (12)
#define M_NEXT (13)
#define M_REPO (14)
#define M_BOOL (15)
#define M_NAME (16)
#define M_SELF (17)
#define M_FATHER (18)
#define M_MESSAGE (19)
#define M_STR (20)
#define M_DOWN_ASSIGMENT (21)
#define M_SLICE_ASSIGMENT (22)
#define M_DOWN_DEL (23)
#define M_SLICE_DEL (24)
#define M_ATTR (25)
#define M_VAL (26)

// TODO-szh 添加val

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

        struct LinkValue *base_obj[BASEOBJSZIE];
        struct LinkValue *base_exc[BASEEXCESIZE];

        wchar_t *var_name[VARNAMESIZE];
        wchar_t *mag_func[MAGFUNCSIZE];

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
