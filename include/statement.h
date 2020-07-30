#ifndef VIRTUALMATH_STATEMENT_H
#define VIRTUALMATH_STATEMENT_H
#include "__macro.h"

typedef struct Statement{
    enum StatementType{
        start = 1,
        base_value,
        base_var,
        operation,
        set_function,
        call_function,
        if_branch,
        while_branch,
        for_branch,
        try_branch,
        with_branch,
        break_cycle,
        continue_cycle,
        rego_if,
        return_code,
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
        struct {
            struct Statement *name;
            struct Statement *function;
        } set_function;
        struct {
            struct Statement *function;
        } call_function;
        struct {
            struct StatementList *if_list;  // if elif
            struct Statement *else_list;  // else分支(无condition)
            struct Statement *finally;
        } if_branch;
        struct {
            enum {
                while_,
                do_while_,
            } type;
            struct Statement *first;  // first do
            struct StatementList *while_list;  // while循环体
            struct Statement *after;  // after do
            struct Statement *else_list;  // else分支(无condition)
            struct Statement *finally;
        } while_branch;
        struct {
            struct Statement *var;  // first do
            struct Statement *iter;  // after do
            struct StatementList *for_list;  // for循环体
            struct Statement *else_list;  // else分支(无condition)
            struct Statement *finally;
        } for_branch;
        struct {
            struct Statement *try;  // first do
            struct StatementList *except_list;  // for循环体
            struct Statement *else_list;  // else分支(无condition)
            struct Statement *finally;
        } try_branch;
        struct {
            struct StatementList *with_list;  // for循环体
            struct Statement *else_list;  // else分支(无condition)
            struct Statement *finally;
        } with_branch;
        struct {
            struct Statement *times;
        } break_cycle;
        struct {
            struct Statement *times;
        } continue_cycle;
        struct {
            struct Statement *times;
        } rego_if;
        struct {
            struct Statement *value;
        } return_code;
    }u;
    struct Statement *next;
} Statement;

typedef struct StatementList{
    enum {
        if_b,
        do_b,
    } type;
    struct Statement *condition;
    struct Statement *var;
    struct Statement *code;
    struct StatementList *next;
} StatementList;

Statement *makeStatement();
Statement *makeOperationStatement(int type);
struct Token *setOperationFromToken(Statement *st, struct Token *left, struct Token *right, int type);

Statement *makeFunctionStatement(Statement *name, Statement *function);
Statement *makeCallStatement(Statement *function);
Statement *makeIfStatement();
Statement *makeWhileStatement();
Statement *makeBreakStatement(Statement *times);
Statement *makeContinueStatement(Statement *times);
Statement *makeRegoStatement(Statement *times);
Statement *makeReturnStatement(Statement *value);

void connectStatement(Statement *base, Statement *new);
void freeStatement(Statement *st);

StatementList *connectStatementList(StatementList *base, StatementList *new);
StatementList *makeStatementList(Statement *condition, Statement *var, Statement *code, int type);
void freeStatementList(StatementList *base);
#define makeConnectStatementList(base, condition, var, code, type) connectStatementList(base, makeStatementList(condition, var, code, type))

#endif //VIRTUALMATH_STATEMENT_H
