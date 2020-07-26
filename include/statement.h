#ifndef VIRTUALMATH_STATEMENT_H
#define VIRTUALMATH_STATEMENT_H
#include "__macro.h"
#define MAX_SIZE (1024)

struct Statement;

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

typedef struct VirtualMathVar{
    char *name;
    struct VirtualMathLinkValue *value;
    struct VirtualMathVar *next;
} Var;

typedef struct VirtualMathHashTable{
    struct VirtualMathVar **hashtable;
    int count;
    struct VirtualMathHashTable *next;
    struct VirtualMathHashTable *last;
} HashTable;

typedef struct VirtualMathVarList{
    struct VirtualMathHashTable *hashtable;
    struct VirtualMathVarList *next;
} VarList;

typedef struct Statement{
    enum StatementType{
        start = 1,
        base_value,
        base_var,
        operation,
    } type;
    union StatementU{
        struct base_value{
            struct VirtualMathValue *value;
        } base_value;
        struct base_var{
            char *name;
            struct Statement *times;
        } base_var;
        struct operation{
            enum OperationType{
                ADD = 1,
                SUB,
                MUL,
                DIV,
                ASS,
            } OperationType;
            struct Statement *left;
            struct Statement *right;
        } operation;
    }u;
    struct Statement *next;
} Statement;

#endif //VIRTUALMATH_STATEMENT_H
