#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H

#include "__macro.h"

struct VarList;
struct Argument;

struct Value{
    GCStatus gc_status;
    enum ValueType{
        none=0,
        number=1,
        string=2,
        function=3,
        list=4,
        dict=5,
    } type;
    union data{
        struct Number{
            NUMBER_TYPE num;
        } num;
        struct String{
            char *str;
        } str;
        struct Function{
            struct Statement *function;
            struct VarList *var;
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
    struct Value *next;
    struct Value *last;
};

struct LinkValue{
    GCStatus gc_status;
    struct Value *value;
    struct LinkValue *father;
    struct LinkValue *next;
    struct LinkValue *last;
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

typedef struct Value Value;
typedef struct LinkValue LinkValue;
typedef struct Result Result;
typedef struct Error Error;

Value *makeValue(Inter *inter);
Value * freeValue(Value *value, Inter *inter);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
LinkValue * freeLinkValue(LinkValue *value, Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);
Value *makeFunctionValue(struct Statement *st, struct Parameter *pt, struct VarList *var_list, Inter *inter);
Value *makeListValue(struct Argument **arg_ad, Inter *inter, enum ListType type);
Value *makeDictValue(struct Argument **arg_ad, bool new_hash, Result *result, Inter *inter, struct VarList *var_list);

void setResultCore(Result *ru);
void setResult(Result *ru, Inter *inter);
void setResultBase(Result *ru, Inter *inter);
void setResultError(Result *ru, Inter *inter, char *error_type, char *error_message, struct Statement *st, bool new);
void setResultOperationNone(Result *ru, Inter *inter);
void setResultOperation(Result *ru, LinkValue *value, Inter *inter);
void setResultOperationBase(Result *ru, LinkValue *value, Inter *inter);
void freeResult(Result *ru);

Error *makeError(char *type, char *message, long int line, char *file);
void freeError(Result *base);
Error *connectError(Error *new, Error *base);
void printError(Result *result, Inter *inter, bool free);

void printValue(Value *value, FILE *debug);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

bool isType(Value *value, enum ValueType type);

#endif //VIRTUALMATH_VALUE_H
