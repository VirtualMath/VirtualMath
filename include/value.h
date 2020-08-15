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
            struct Statement *function;
            struct Parameter *pt;
        } function;
        struct List{
            enum ListType{
                value_tuple,
                value_list,
            } type;
            struct LinkValue **list;
            long int size;
        } list;
        struct Dict{
            struct HashTable *dict;
            NUMBER_TYPE size;
        } dict;
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
        function_return,  // 函数返回值
        operation_return,  // 表达式返回值
        error_return,  // 错误
        break_return,
        continue_return,
        rego_return,
        restart_return,
    } type;
    struct LinkValue *value;
    struct Error *error;
    int times;
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
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);
Value *makeFunctionValue(struct Statement *st, struct Parameter *pt, struct VarList *var_list, Inter *inter);
Value *makeClassValue(VarList *var_list, Inter *inter, FatherValue *father);
Value *makeListValue(struct Argument **arg_ad, Inter *inter, enum ListType type);
Value *makeDictValue(struct Argument **arg_ad, bool new_hash, INTER_FUNCTIONSIG_NOT_ST);

void setResultCore(Result *ru);
void setResult(Result *ru, Inter *inter, LinkValue *father);
void setResultBase(Result *ru, Inter *inter, LinkValue *father);
void setResultError(Result *ru, Inter *inter, char *error_type, char *error_message, Statement *st, LinkValue *father,
                    bool new);
void setResultOperationNone(Result *ru, Inter *inter, LinkValue *father);
void setResultOperation(Result *ru, LinkValue *value, Inter *inter);
void setResultOperationBase(Result *ru, LinkValue *value, Inter *inter);
void freeResult(Result *ru);
void freeResultSave(Result *ru);

Error *makeError(char *type, char *message, long int line, char *file);
void freeError(Result *base);
Error *connectError(Error *new, Error *base);
void printError(Result *result, Inter *inter, bool free);

void printValue(Value *value, FILE *debug);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

bool isType(Value *value, enum ValueType type);

FatherValue *makeFatherValue(LinkValue *value);
FatherValue *copyFatherValue(FatherValue *value);
FatherValue *freeFatherValue(FatherValue *value);
FatherValue *connectFatherValue(FatherValue *base, FatherValue *back);
FatherValue *connectSafeFatherValue(FatherValue *base, FatherValue *back);
bool checkAttribution(Value *self, Value *father);
#endif //VIRTUALMATH_VALUE_H
