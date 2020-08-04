#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H
#include "__macro.h"

struct VarList;
struct Argument;

struct Value{
    enum ValueType{
        none=0,
        number=1,
        string,
        function,
        list,
        dict,
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
    int times;
};

typedef struct Value Value;
typedef struct LinkValue LinkValue;
typedef struct Result Result;

Value *makeValue(Inter *inter);
void freeValue(Value *value, Inter *inter);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
void freeLinkValue(LinkValue *value, Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);
Value *makeFunctionValue(struct Statement *st, struct Parameter *pt, struct VarList *var_list, Inter *inter);
Value *makeListValue(struct Argument **arg_ad, Inter *inter, enum ListType type);
Value *makeDictValue(struct Argument **arg_ad, bool new_hash, Inter *inter);

void setResult(Result *ru, bool link, Inter *inter);
void setResultError(Result *ru, Inter *inter);
void setResultOperation(Result *ru, Inter *inter);

void printValue(Value *value, FILE *debug);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

#endif //VIRTUALMATH_VALUE_H
