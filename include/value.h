#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H
#include "inter.h"

typedef struct VirtualMathValue{
    enum ValueType{
        number=1,
        string,
    } type;
    union data{
        struct Number{
            NUMBER_TYPE num;
        } num;
        struct String{
            char *str;
        } str;
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
        statement_end = 1,
    } type;
    struct VirtualMathLinkValue *value;
} Result;

Value *makeValue(Inter *inter);
void freeValue(Value *value, Inter *inter);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
void freeLinkValue(LinkValue *value, Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);

void setResult(Result *ru, bool link, Inter *inter);


#endif //VIRTUALMATH_VALUE_H
