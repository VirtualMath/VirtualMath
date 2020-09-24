#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H
#include "__macro.h"

// 标准错误信息定义
#define INSTANCE_ERROR(class) "Instance error when calling function, call non-"#class" "#class" method"
#define VALUE_ERROR(value, acc) #value" value is not a "#acc" (may be modified by an external program)"
#define ONLY_ACC(var, value) #var" only accepts "#value
#define ERROR_INIT(class) #class" get wrong initialization parameters"
#define MANY_ARG "Too many parameters"
#define FEW_ARG "Too few parameters"
#define CUL_ERROR(opt) #opt" operation gets incorrect value"
#define OBJ_NOTSUPPORT(opt) "Object does not support "#opt" operation"
#define RETURN_ERROR(func, type) #func" function should return "#type" type data"
#define KEY_INTERRUPT "KeyInterrupt"

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

typedef enum ResultType (*OfficialFunction)(OFFICAL_FUNCTIONSIG);
typedef void (*Registered)(REGISTERED_FUNCTIONSIG);

enum ValueAuthority {
    auto_aut,
    public_aut,
    protect_aut,
    private_aut
};

enum ValueType {
    none=0,
    number=1,
    string=2,
    function=3,
    list=4,
    dict=5,
    class=6,
    object_=7,
    bool_=8,
    pass_=9,
};

struct Number {
    vnum num;
};

struct String {
    wchar_t *str;
};

struct Function{
    enum {
        c_function,
        vm_function,
    } type;
    struct Statement *function;
    struct Parameter *pt;
    OfficialFunction of;
    struct {
        enum FunctionPtType {
            free_,  // 不包含任何隐式传递的参数
            static_,  // 不包含self参数
            object_static_,  // self参数不允许class
            class_static_,  // self参数允许一切，但转换为类
            all_static_, // self参数允许一切
            cls_static_,  // 使用function自带的cls作为参数
            object_free_,  // 同object_static_但不包含func参数
            class_free_,  // 同object_static_但不包含func参数
            all_free_,  // 允许class或者object
            cls_free_,  // 使用function自带的cls作为参数
        } pt_type;
        LinkValue *cls;
    } function_data;
};

struct List {
    enum ListType {
        value_tuple,
        value_list,
    } type;
    struct LinkValue **list;
    vnum size;
};

struct Dict {
    struct HashTable *dict;
    vnum size;
};

struct Bool{
    bool bool_;
};

struct Value{
    enum ValueType type;

    struct {
        struct VarList *var;
        struct VarList *out_var;
        struct Inherit *inherit;
    } object;

    union data {
        struct Number num;
        struct String str;
        struct Function function;
        struct List list;
        struct Dict dict;
        struct Bool bool_;
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
        not_return = 1,  // 无返回值
        function_return=2,  // 函数返回值
        operation_return=3,  // 表达式返回值
        error_return=4,  // 错误
        break_return=5,
        continue_return=6,
        rego_return=7,
        restart_return=8,
        goto_return=9,
        yield_return=10,
    } type;
    wchar_t *label;
    struct LinkValue *value;
    struct Error *error;
    vnum times;
    struct Statement *node;
};

struct Error {
    char *type;
    char *messgae;
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

enum BaseErrorType{
    E_BaseException,
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
};

Value *makeObject(Inter *inter, VarList *object, VarList *out_var, Inherit *inherit);
void freeValue(Value **Value);
LinkValue *makeLinkValue(Value *value, LinkValue *belong, Inter *inter);
void freeLinkValue(LinkValue **value);
LinkValue *copyLinkValue(LinkValue *value, Inter *inter);
Value *useNoneValue(Inter *inter, Result *result);
Value *makeBoolValue(bool bool_num, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
Value *makePassValue(fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
Value *makeNumberValue(vnum num, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
Value *makeStringValue(wchar_t *str, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
Value *makeVMFunctionValue(struct Statement *st, struct Parameter *pt, INTER_FUNCTIONSIG_NOT_ST);
Value *makeCFunctionValue(OfficialFunction of, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);
LinkValue *makeCFunctionFromOf(OfficialFunction of, LinkValue *func, OfficialFunction function_new, OfficialFunction function_init, LinkValue *belong, VarList *var_list, Inter *inter);
Value *makeClassValue(VarList *var_list, Inter *inter, Inherit *father);
Value *makeListValue(Argument *arg, fline line, char *file, enum ListType type, INTER_FUNCTIONSIG_NOT_ST);
Value *makeDictValue(Argument *arg, bool new_hash, fline line, char *file, INTER_FUNCTIONSIG_NOT_ST);

void setResultCore(Result *ru);
void setResult(Result *ru, Inter *inter, LinkValue *belong);
void setResultBase(Result *ru, Inter *inter);
void setResultErrorSt(BaseErrorType type, char *error_message, bool new, INTER_FUNCTIONSIG);
void setResultError(BaseErrorType type, char *error_message, fline line, char *file, bool new, INTER_FUNCTIONSIG_NOT_ST);
void setResultOperationNone(Result *ru, Inter *inter, LinkValue *belong);
void setResultOperation(Result *ru, LinkValue *value);
void setResultOperationBase(Result *ru, LinkValue *value);
void freeResult(Result *ru);
void freeResultSafe(Result *ru);

Package *makePackage(Value *value, char *md5, char *name, Package *base);
void freePackage(Package *base);
Value *checkPackage(Package *base, char *md5, char *name);

Error *makeError(char *type, char *message, fline line, char *file);
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
