#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H
#include "__macro.h"

// 标准错误信息定义
#define INSTANCE_ERROR(class)  (wchar_t *) L"instance error when calling func, call non-" L###class L" " L###class L" method"
#define VALUE_ERROR(value, acc) (wchar_t *) L###value L" value is not a " L###acc L" (may be modified by an external program)"
#define ONLY_ACC(var, value) (wchar_t *) L###var L" only accepts " L###value
#define ERROR_INIT(class) (wchar_t *) L###class L" get wrong initialization parameters"
#define MANY_ARG (wchar_t *) L"too many parameters"
#define FEW_ARG (wchar_t *) L"too few parameters"
#define CUL_ERROR(opt) (wchar_t *) L###opt L" operation gets incorrect value"
#define OBJ_NOTSUPPORT(opt) (wchar_t *) L"object does not support " L###opt L" operation"
#define RETURN_ERROR(func, type) (wchar_t *) L###func L" func should return " L###type L" type data"
#define KEY_INTERRUPT (wchar_t *) L"keyInterrupt"
#define NOT_ENOUGH_LEN(for) L"not enough length for" L###for

#define GET_RESULT(val, res) do {(val) = (res)->value; (res)->value=NULL; freeResult(res);} while(0)
#define GET_RESULTONLY(val, res) do {(val) = (res)->value; (res)->value=NULL;} while(0)

#define COPY_LINKVALUE(val, inter) makeLinkValue((val)->value, (val)->belong, (val)->aut, (inter))

#define NORMAL_BUILTIN(val) ((val)->type != V_obj && (val)->type != V_class)
#define SIMPLE_BUILTIN(val, inter) (((val)->object.inherit != NULL && (val)->object.inherit->next != NULL) && (val)->object.inherit->next->value->value == (inter)->data.base_obj[B_VOBJECT]->value)
#define IS_BUILTIN_VALUE(val, inter) (((inter)->data.opt_mode == om_normal && NORMAL_BUILTIN(val)) || (inter->data.opt_mode == om_simple && NORMAL_BUILTIN(val) && SIMPLE_BUILTIN(val, inter)))

typedef struct Argument Argument;
typedef struct Inter Inter;
typedef struct VarList VarList;
typedef enum ResultType ResultType;
typedef enum BaseErrorType BaseErrorType;

typedef struct Value Value;
typedef struct LinkValue LinkValue;
typedef struct Result Result;
typedef struct Error Error;
typedef struct Inherit Inherit;
typedef struct Package Package;
typedef struct SignalList SignalList;

typedef enum ResultType (*OfficialFunction)(O_FUNC);
typedef void (*Registered)(R_FUNC);

enum ValueAuthority {
    auto_aut,
    public_aut,
    protect_aut,
    private_aut
};

enum ValueType {
    V_none=0,
    V_int=1,
    V_dou=2,
    V_str=3,
    V_func=4,
    V_list=5,
    V_dict=6,
    V_class=7,
    V_obj=8,
    V_bool=9,
    V_ell=10,
    V_file=11,
    V_lib=12,
    V_pointer=13,
    V_struct=14,
};

struct Int {
    vint num;
};

struct Dou {
    vdou num;
};

struct String {
    wchar_t *str;
};

struct Struct_ {
    vstruct *data;  // 列表
    vint len;  // 长度
};

struct Function{
    enum {
        c_func,
        vm_func,
        f_func,
    } type;
    struct Statement *function;
    struct Parameter *pt;
    OfficialFunction of;
    void (*ffunc)();  // ffi导入的func
    struct {
        enum FunctionPtType {
            fp_no_,  // 不包含任何隐式传递的参数
            fp_func_,  // 不包含self参数
            fp_func_obj,  // self参数不允许class
            fp_func_class,  // self参数允许一切，但转换为类
            fp_func_all, // self参数允许一切
            fp_func_cls,  // 使用function自带的cls作为参数
            fp_obj,  // 同object_static_但不包含func参数
            fp_class,  // 同object_static_但不包含func参数
            fp_all,  // 允许class或者object
            fp_cls,  // 使用function自带的cls作为参数

            // 组合
            fp_func_cls_obj,
            fp_func_cls_class,
            fp_func_cls_all,

            fp_cls_obj,
            fp_cls_class,
            fp_cls_all,
            // obj和class没有同时存在的意义, 可以使用内置函数直接获取obj所对应的class
            // 直接设定class的目的是修饰类方法
        } pt_type;
        LinkValue *cls;
        bool run;  // 是否为即时调用
        bool push;  // 是否需要push var
    } function_data;
};

struct List {
    enum ListType {
        L_tuple,
        L_list,
    } type;
    struct LinkValue **list;
    vint size;
};

struct Dict {
    struct HashTable *dict;
    vint size;
};

struct Bool{
    bool bool_;
};

struct Lib{
    void *handle;
};

struct File{
    FILE *file;
    char *path;  // 文件路径
    char *mode;  // 模式
    bool is_std;
};

struct Pointer {
    void *pointer;
};

struct Value{
    enum ValueType type;

    struct {
        struct VarList *var;
        struct VarList *out_var;
        struct Inherit *inherit;
    } object;

    union data {
        struct Int int_;
        struct Dou dou;
        struct String str;
        struct Function function;
        struct List list;
        struct Dict dict;
        struct Bool bool_;
        struct File file;
        struct Lib lib;
        struct Pointer pointer;
        struct Struct_ struct_;
    } data;

    struct Value *gc_next;
    struct Value *gc_last;
    struct GCStatus gc_status;
};

struct LinkValue {
    enum ValueAuthority aut;
    struct Value *value;
    struct LinkValue *belong;
    struct LinkValue *gc_next;
    struct LinkValue *gc_last;
    struct GCStatus gc_status;
};

struct Result {
    enum ResultType {
        R_not = 1,  // 无返回值
        R_func=2,  // 函数返回值
        R_opt=3,  // 表达式返回值
        R_error=4,  // 错误
        R_break=5,
        R_continue=6,
        R_rego=7,
        R_restart=8,
        R_goto=9,
        R_yield=10,
    } type;
    wchar_t *label;
    struct LinkValue *value;
    struct Error *error;
    vint times;
    struct Statement *node;
    bool is_yield;  // 执行的函数是否为生成器
};

struct Error {
    wchar_t *type;
    wchar_t *messgae;
    char *file;
    fline line;
    struct Error *next;
};

struct Inherit {
    struct LinkValue *value;
    struct Inherit *next;
};

struct Package {
    struct Value *package;
    char *name;  // split dir的name
    char *md5;  // md5地址
    struct Package *next;
};

struct SignalList {
    vsignal sig_num;  // 信号
    LinkValue *value;  // 处理函数
    struct SignalList *next;
};

enum BaseErrorType{
    E_BaseException = 0,
    E_Exception,
    E_TypeException,
    E_ArgumentException,
    E_PermissionsException,
    E_GotoException,
    E_ResultException,
    E_NameExceptiom,
    E_AssertException,
    E_KeyException,
    E_IndexException,
    E_StrideException,
    E_StopIterException,
    E_SuperException,
    E_ImportException,
    E_IncludeException,
    E_SystemException,
    E_KeyInterrupt,
    E_QuitException,
    E_ValueException,
};

Value *makeObject(Inter *inter, VarList *object, VarList *out_var, bool set_out_var, Inherit *inherit);
void freeValue(Value **Value);
LinkValue *makeLinkValue(Value *value, LinkValue *belong, enum ValueAuthority aut, Inter *inter);
void freeLinkValue(LinkValue **value);
Value *useNoneValue(Inter *inter, Result *result);
Value *makeBoolValue(bool bool_num, fline line, char *file, FUNC_NT);
Value *makeStructValue(void *data, vint len, fline line, char *file, FUNC_NT);
Value *makePassValue(fline line, char *file, FUNC_NT);
Value *makeIntValue(vint num, fline line, char *file, FUNC_NT);
Value *makeDouValue(vdou num, fline line, char *file, FUNC_NT);
Value *makePointerValue(void *p, fline line, char *file, FUNC_NT);
Value *makeStringValue(wchar_t *str, fline line, char *file, FUNC_NT);
Value *makeFileValue(FILE *file_, char *mode, bool is_std, char *path, fline line, char *file, FUNC_NT);
Value *makeVMFunctionValue(struct Statement *st, struct Parameter *pt, FUNC_NT);
Value *makeCFunctionValue(OfficialFunction of, fline line, char *file, bool set_var, bool push, FUNC_NT);
LinkValue *makeCFunctionFromOf(OfficialFunction of, LinkValue *func, OfficialFunction function_new, LinkValue *belong, VarList *var_list, Inter *inter);
Value *makeFFunctionValue(void (*ffunc)(), fline line, char *file, FUNC_NT);
Value *makeClassValue(VarList *var_list, Inter *inter, Inherit *father);
Value *makeListValue(Argument *arg, fline line, char *file, enum ListType type, FUNC_NT);
Value *makeDictValue(Argument *arg, bool new_hash, fline line, char *file, FUNC_NT);

void setResultCore(Result *ru);
void setResult(Result *ru, Inter *inter);
void setResultBase(Result *ru, Inter *inter);
void setResultErrorSt(BaseErrorType type, wchar_t *error_message, bool new, FUNC);
void setResultFromERR(enum BaseErrorType exc, FUNC_NT);
void setResultError(BaseErrorType type, wchar_t *error_message, fline line, char *file, bool new, FUNC_NT);
void setResultOperationNone(Result *ru, Inter *inter, LinkValue *belong);
void setResultOperation(Result *ru, LinkValue *value);
void setResultOperationBase(Result *ru, LinkValue *value);
void freeResult(Result *ru);

Package *makePackage(Value *value, char *md5, char *name, Package *base);
void freePackage(Package *base);
Value *checkPackage(Package *base, char *md5, char *name);

SignalList *makeSignalList(vsignal sig_num, LinkValue *value);
LinkValue *exchangeSignalFunc(SignalList *sig_list, LinkValue *new);
SignalList *freeSignalList(SignalList *sig_list);
SignalList *checkSignalList(vsignal sig_num, SignalList *sig_list);
LinkValue *delSignalList(vsignal sig_num, SignalList **sig_list);

Error *makeError(wchar_t *type, wchar_t *message, fline line, char *file);
void freeError(Result *base);
Error *connectError(Error *new, Error *base);
void printError(Result *result, Inter *inter, bool free);

void printValue(Value *value, FILE *debug, bool print_father, bool print_in);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

bool isType(Value *value, enum ValueType type);

Inherit *makeInherit(LinkValue *value);
Inherit *copyInherit(Inherit *value);
Inherit *freeInherit(Inherit *value);
Inherit *connectInherit(Inherit *base, Inherit *back);
Inherit *connectSafeInherit(Inherit *base, Inherit *back);
bool checkAttribution(Value *self, Value *father);

Inherit *getInheritFromValueCore(LinkValue *num_father);
bool callDel(Value *object_value, Result *result, Inter *inter, VarList *var_list);
bool needDel(Value *object_value, Inter *inter);
#endif //VIRTUALMATH_VALUE_H
