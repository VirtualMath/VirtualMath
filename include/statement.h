#ifndef VIRTUALMATH_STATEMENT_H
#define VIRTUALMATH_STATEMENT_H
#include "__macro.h"

struct Statement{
    enum ValueAuthority aut;
    enum StatementType{
        start = 1,
        base_value,
        base_list,
        base_dict,
        base_var,
        base_svar,
        operation,
        set_function,
        set_class,
        call_function,
        if_branch,
        while_branch,
        for_branch,
        try_branch,
        with_branch,
        break_cycle,
        continue_cycle,
        rego_if,
        restart,
        return_code,
        raise_code,
        include_file,
        import_file,
        from_import_file,
    } type;
    union StatementU{
        struct base_value{
            enum BaseValueType{
                link_value = 0,
                string_str = 1,
                number_str = 2,
            } type;
            struct LinkValue *value;
            char *str;
        } base_value;
        struct base_var{
            char *name;
            struct Statement *times;
        } base_var;
        struct base_svar{
            struct Statement *name;
            struct Statement *times;
        } base_svar;
        struct {
            enum ListType type;
            struct Parameter *list;
        } base_list;
        struct {
            struct Parameter *dict;
        } base_dict;
        struct operation{
            enum OperationType{
                OPT_ADD = 1,
                OPT_SUB = 2,
                OPT_MUL = 3,
                OPT_DIV = 4,
                OPT_ASS = 5,
                OPT_POINT = 6,
            } OperationType;
            struct Statement *left;
            struct Statement *right;
        } operation;
        struct {
            struct Statement *name;
            struct Statement *function;
            struct Parameter *parameter;
        } set_function;
        struct {
            struct Statement *name;
            struct Statement *st;
            struct Parameter *father;
        } set_class;
        struct {
            struct Statement *function;
            struct Parameter *parameter;
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
            struct Statement *times;
        } restart;
        struct {
            struct Statement *value;
        } return_code;
        struct {
            struct Statement *value;
        } raise_code;
        struct {
            struct Statement *file;
        } include_file;
        struct {
            struct Statement *file;
            struct Statement *as;
        } import_file;
        struct {
            struct Statement *file;
            struct Parameter *pt;
            struct Parameter *as;
        } from_import_file;
    }u;
    long int line;
    char *code_file;
    struct Statement *next;
};

struct StatementList{
    enum {
        if_b,
        do_b,
        while_b,
        except_b,
    } type;
    struct Statement *condition;
    struct Statement *var;
    struct Statement *code;
    struct StatementList *next;
};

typedef struct Statement Statement;
typedef struct StatementList StatementList;

Statement *makeStatement(long int line, char *file);
void freeStatement(Statement *st);
Statement *copyStatement(Statement *st);
Statement *copyStatementCore(Statement *st);
void connectStatement(Statement *base, Statement *new);

Statement *makeOperationStatement(int type, long int line, char *file);
Statement *makeBaseLinkValueStatement(LinkValue *value, long int line, char *file);
Statement *makeBaseStrValueStatement(char *value, enum BaseValueType type, long int line, char *file);
Statement *makeBaseVarStatement(char *name, Statement *times, long int line, char *file);
Statement *makeBaseSVarStatement(Statement *name, Statement *times);
Statement *makeBaseDictStatement(Parameter *pt, long int line, char *file);
Statement *makeTupleStatement(Parameter *pt, enum ListType type, long int line, char *file);
Statement *makeClassStatement(Statement *name, Statement *function, Parameter *pt);
Statement *makeFunctionStatement(Statement *name, Statement *function, struct Parameter *pt);
Statement *makeCallStatement(Statement *function, struct Parameter *pt);
Statement *makeIfStatement(long int line, char *file);
Statement *makeWhileStatement(long int line, char *file);
Statement *makeTryStatement(long int line, char *file);
Statement *makeBreakStatement(Statement *times, long int line, char *file);
Statement *makeContinueStatement(Statement *times, long int line, char *file);
Statement *makeRegoStatement(Statement *times, long int line, char *file);
Statement *makeRestartStatement(Statement *times, long int line, char *file);
Statement *makeReturnStatement(Statement *value, long int line, char *file);
Statement *makeRaiseStatement(Statement *value, long int line, char *file);
Statement *makeIncludeStatement(Statement *file, long int line, char *file_dir);
Statement *makeImportStatement(Statement *file, Statement *as);
Statement *makeFromImportStatement(Statement *file, Parameter *as, Parameter *pt);
struct Token *setOperationFromToken(Statement **st_ad, struct Token *left, struct Token *right, enum OperationType type, bool is_right);

StatementList *makeStatementList(Statement *condition, Statement *var, Statement *code, int type);
StatementList *connectStatementList(StatementList *base, StatementList *new);
void freeStatementList(StatementList *base);
StatementList *copyStatementList(StatementList *sl);
#define makeConnectStatementList(base, condition, var, code, type) connectStatementList(base, makeStatementList(condition, var, code, type))

#endif //VIRTUALMATH_STATEMENT_H
