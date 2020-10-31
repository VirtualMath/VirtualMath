#ifndef VIRTUALMATH_INTER_H
#define VIRTUALMATH_INTER_H

struct Result;

/* 100 大概是最好的值 */
#define RUNGC (100)
#define SHOULD_RUNGC(inter) ((inter)->data.start_gc && (inter)->data.run_gc >= RUNGC)

#define BASEOBJSZIE (17)
#define VARNAMESIZE (10)
#define BASEEXCESIZE (20)
#define MAGFUNCSIZE (46)

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
#define VN_dict (8)
#define VN_tuple (9)

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

#define M_INTDIV (27)
#define M_MOD (28)
#define M_POW (29)

#define M_BAND (30)
#define M_BOR (31)
#define M_BXOR (32)
#define M_BNOT (33)
#define M_BL (34)
#define M_BR (35)

#define M_EQ (36)
#define M_MOREEQ (37)
#define M_LESSEQ (38)
#define M_MORE (39)
#define M_LESS (40)
#define M_NOTEQ (41)

#define M_AND (42)
#define M_OR (43)
#define M_NOT (44)
#define M_NEGATE (45)

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
        size_t var_max;
        int var_deep;
        enum AssertRunType {
            assert_ignore,  // 忽略一切
            assert_run,  // 仅执行
            assert_raise,  // 执行并生效
        } assert_run;
        int run_gc;  // gc的启动计数
        bool start_gc;  // 是否启动gc
        bool free_mode;  // 自由模式(若为true, 则在makeValue的使用完全通过callBack执行)
        enum OptMode {
            om_free,  // 完全通过callBack执行
            om_normal,  // 只要ValueType不是obj或class就通过静态方法执行
            om_simple,  // buildin 类型都通过静态方法执行
        } opt_mode;  // 表达式执行模式

        bool value_folding;  // 常量折叠[on]
        bool var_folding;  // 变量折叠[off]
        bool opt_folding;  // 表达式折叠[on]
        bool cyc_folding;  // 在循环内部自动打开折叠
        bool func_folding;  // 在函数内部自动打开
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
