#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H
#include "inter.h"

typedef struct VirtualMathValue{
    enum ValueType{
        none=0,
        number=1,
        string,
        function,
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
        } function;
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
Value *makeFunctionValue(Statement *st, struct VirtualMathVarList *var_list, Inter *inter);

void setResult(Result *ru, bool link, Inter *inter);
void setResultError(Result *ru, Inter *inter);
void setResultOperation(Result *ru, Inter *inter);

#endif //VIRTUALMATH_VALUE_H
