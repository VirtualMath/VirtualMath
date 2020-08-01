#ifndef VIRTUALMATH_VALUE_H
#define VIRTUALMATH_VALUE_H

typedef struct Value{
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
            struct VarList *var;
            struct Parameter *pt;
        } function;
        struct {
            enum {
                value_tuple,
                value_list,
            } type;
            struct LinkValue **list;
            long int size;
        } list;
    }data;
    struct Value *next;
    struct Value *last;
} Value;

typedef struct LinkValue{
    struct Value *value;
    struct LinkValue *father;
    struct LinkValue *next;
    struct LinkValue *last;
} LinkValue;

typedef struct Result{
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
} Result;

struct VarList;
struct Argument;

Value *makeValue(Inter *inter);
void freeValue(Value *value, Inter *inter);
LinkValue *makeLinkValue(Value *value, LinkValue *linkValue,Inter *inter);
void freeLinkValue(LinkValue *value, Inter *inter);
Value *makeNumberValue(long num, Inter *inter);
Value *makeStringValue(char *str, Inter *inter);
Value *makeFunctionValue(struct Statement *st, struct Parameter *pt, struct VarList *var_list, Inter *inter);
Value *makeListValue(struct Argument **ad_ad, Inter *inter, int type);

void setResult(Result *ru, bool link, Inter *inter);
void setResultError(Result *ru, Inter *inter);
void setResultOperation(Result *ru, Inter *inter);

void printValue(Value *value, FILE *debug);
void printLinkValue(LinkValue *value, char *first, char *last, FILE *debug);

#endif //VIRTUALMATH_VALUE_H
