#ifndef VIRTUALMATH_STATEMENT_H
#define VIRTUALMATH_STATEMENT_H
#include "__macro.h"

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

Statement *makeStatement();
Statement *makeOperationStatement(int type);
struct Token *setOperationFromToken(Statement *st, struct Token *left, struct Token *right, int type);

void connectStatement(Statement *base, Statement *new);
void freeStatement(Statement *st);

#endif //VIRTUALMATH_STATEMENT_H
