#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H

#include "__macro.h"
#include "gc.h"
#include "run.h"

struct VarList;
struct Argument;
struct FatherValue;

enum ValueAuthority{
    auto_aut,
    public_aut,
    protect_aut,
    private_aut
};

struct Value{
    struct GCStatus gc_status;
    enum ValueType{
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
    } type;
    struct {
        struct VarList *var;
        struct VarList *out_var;
        struct FatherValue *father;
    } object;
    union data{
        struct Number{
            NUMBER_TYPE num;
        } num;
        struct String{
            char *str;
        } str;
        struct Function{
            enum{
                c_function,
                vm_function,
            } type;
            struct Statement *function;
            struct Parameter *pt;
            OfficialFunction of;
            struct {
                enum {
                    static_,
                    object_static_,
                    class_static_,
                } pt_type;
            } function_data;
        } function;
        struct List{
            enum ListType{
                value_tuple,
                value_list,
            } type;
            struct LinkValue **list;
            NUMBER_TYPE size;  // TODO-szh typedef NUMBER_TYPE
        } list;
        struct Dict{
            struct HashTable *dict;
            NUMBER_TYPE size;
        } dict;
        struct Bool{
            bool bool_;
        } bool_;
    }data;
    struct Value *gc_next;
    struct Value *gc_last;
};

struct LinkValue{
    struct GCStatus gc_status;
    enum ValueAuthority aut;
    struct Value *value;
    struct LinkValue *father;
    struct LinkValue *gc_next;
    struct LinkValue *gc_last;
};

struct Result{
    enum ResultType{
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
    char *label;
    struct LinkValue *value;
    struct Error *error;
    int times;
    struct Statement *node;
};

struct Error{
    char *type;
    char *messgae;
    char *file;
    long int line;
    struct Error *next;
};

struct FatherValue{
    struct LinkValue *value;
    struct FatherValue *next;
};

typedef struct Inter Inter;
typedef struct Value Value;
typedef struct LinkValue LinkValue;
typedef struct Result Result;
typedef struct Error Error;
typedef struct FatherValue FatherValue;
typedef enum ResultType ResultType;

Value *makeObject(Inter *inter, VarList *object, VarList *out_var, FatherValue *father);
void freeValue(Value **Value);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
void freeLinkValue(LinkValue **value);
LinkValue *copyLinkValue(LinkValue *value, Inter *inter);
Value *makeNoneValue(Inter *inter);
Value *makeBoolValue(bool bool_num, Inter *inter);
Value *makePassValue(Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);
Value *makeVMFunctionValue(struct Statement *st, struct Parameter *pt, struct VarList *var_list, Inter *inter);
Value *makeCFunctionValue(OfficialFunction of, VarList *var_list, Inter *inter);
Value *makeClassValue(VarList *var_list, Inter *inter, FatherValue *father);
Value *makeListValue(struct Argument **arg_ad, Inter *inter, enum ListType type);
Value *makeDictValue(struct Argument **arg_ad, bool new_hash, INTER_FUNCTIONSIG_NOT_ST);

void setResultCore(Result *ru);
void setResult(Result *ru, Inter *inter, LinkValue *father);
void setResultBase(Result *ru, Inter *inter, LinkValue *father);
void setResultErrorSt(Result *ru, Inter *inter, char *error_type, char *error_message, Statement *st, LinkValue *father, bool new);
void setResultError(Result *ru, Inter *inter, char *error_type, char *error_message, long int line, char *file, LinkValue *father, bool new);
void setResultOperationNone(Result *ru, Inter *inter, LinkValue *father);
void setResultOperation(Result *ru, LinkValue *value);
void setResultOperationBase(Result *ru, LinkValue *value);
void freeResult(Result *ru);
void freeResultSafe(Result *ru);

Error *makeError(char *type, char *message, long int line, char *file);
void freeError(Result *base);
Error *connectError(Error *new, Error *base);
void printError(Result *result, Inter *inter, bool free);

void printValue(Value *value, FILE *debug, bool print_father);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

bool isType(Value *value, enum ValueType type);

FatherValue *makeFatherValue(LinkValue *value);
FatherValue *copyFatherValue(FatherValue *value);
FatherValue *freeFatherValue(FatherValue *value);
FatherValue *connectFatherValue(FatherValue *base, FatherValue *back);
FatherValue *connectSafeFatherValue(FatherValue *base, FatherValue *back);
bool checkAttribution(Value *self, Value *father);
#endif //VIRTUALMATH_VALUE_H
