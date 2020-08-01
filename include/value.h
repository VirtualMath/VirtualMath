#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H
#include "inter.h"

typedef struct VirtualMathValue{
    enum ValueType{
        none=0,
        number=1,
        string,
        function,
        list,
    } type;
    union data{
        struct Number{
            NUMBER_TYPE num;
        } num;
        struct String{
            char *str;
        } str;
        struct {
            struct Statement *function;
            struct VirtualMathVarList *var;
            struct Parameter *pt;
        } function;
        struct {
            enum {
                value_tuple,
                value_list,
            } type;
            struct VirtualMathLinkValue **list;
            long int size;
        } list;
    }data;
    struct VirtualMathValue *next;
    struct VirtualMathValue *last;
} Value;

typedef struct VirtualMathLinkValue{
    struct VirtualMathValue *value;
    struct VirtualMathLinkValue *father;
    struct VirtualMathLinkValue *next;
    struct VirtualMathLinkValue *last;
} LinkValue;

typedef struct VirtualMathResult{
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
    struct VirtualMathLinkValue *value;
    int times;
} Result;


Value *makeValue(Inter *inter);
void freeValue(Value *value, Inter *inter);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
void freeLinkValue(LinkValue *value, Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);
Value *makeFunctionValue(Statement *st, struct Parameter *pt, struct VirtualMathVarList *var_list, Inter *inter);
Value *makeListValue(struct Parameter **pt_ad, Result *result_tmp, struct globalInterpreter *inter, struct VirtualMathVarList *var_list,
                     int type);

void setResult(Result *ru, bool link, Inter *inter);
void setResultError(Result *ru, Inter *inter);
void setResultOperation(Result *ru, Inter *inter);

void printValue(Value *value, FILE *debug);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

#endif //VIRTUALMATH_VALUE_H
