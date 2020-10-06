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
        del_,
        base_svar,
        base_lambda,
        operation,
        set_function,
        set_class,
        call_function,
        slice_,
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
        yield_code,
        raise_code,
        include_file,
        import_file,
        from_import_file,
        default_var,
        assert,
        label_,
        goto_,
    } type;
    union StatementU{
        struct base_value{
            enum BaseValueType{
                link_value = 0,
                string_str = 1,
                number_str = 2,
                bool_true = 3,
                bool_false = 4,
                pass_value = 5,
                null_value = 6,
            } type;
            struct LinkValue *value;
            wchar_t *str;
        } base_value;
        struct base_var{
            wchar_t *name;
            struct Statement *times;
            bool run;
        } base_var;
        struct{
            struct Statement *var;
        } del_;
        struct base_svar{
            struct Statement *name;
            struct Statement *times;
            bool run;
        } base_svar;
        struct {
            enum ListType type;
            struct Parameter *list;
        } base_list;
        struct {
            struct Parameter *dict;
        } base_dict;
        struct {
            struct Parameter *parameter;
            struct Statement *function;
        } base_lambda;
        struct operation{
            enum OperationType{
                OPT_ADD = 1,
                OPT_SUB = 2,
                OPT_MUL = 3,
                OPT_DIV = 4,
                OPT_ASS = 5,
                OPT_POINT = 6,  // 成员运算
                OPT_BLOCK = 7,  // 代码块
                OPT_LINK = 8,  // 获取外部成员 TODO-szh 该名为OUTPOINT
                OPT_INTDIV = 9,
                OPT_MOD = 10,
                OPT_POW = 11,

                OPT_BAND = 12,
                OPT_BOR = 13,
                OPT_BXOR = 14,
                OPT_BNOT = 15,
                OPT_BL = 16,
                OPT_BR = 17,

                OPT_EQ = 18,
                OPT_MOREEQ = 19,
                OPT_LESSEQ = 20,
                OPT_MORE = 21,
                OPT_LESS = 22,
                OPT_NOTEQ = 23,

                OPT_AND = 24,
                OPT_OR = 25,
                OPT_NOT = 26,
            } OperationType;
            struct Statement *left;
            struct Statement *right;
        } operation;
        struct {
            struct Statement *name;
            struct Statement *function;
            struct Statement *first_do;
            struct Parameter *parameter;
            struct DecorationStatement *decoration;
        } set_function;
        struct {
            struct Statement *name;
            struct Statement *st;
            struct Parameter *father;
            struct DecorationStatement *decoration;
        } set_class;
        struct {
            struct Statement *function;
            struct Parameter *parameter;
        } call_function;
        struct {
            struct Statement *element;
            struct Parameter *index;
            enum SliceType{
                SliceType_down_,
                SliceType_slice_,
            } type;
        } slice_;
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
            struct StatementList *for_list;  // for循环体
            struct Statement *first_do;
            struct Statement *after_do;
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
        } yield_code;
        struct {
            struct Statement *value;
        } raise_code;
        struct {
            struct Statement *file;
        } include_file;
        struct {
            struct Statement *file;
            struct Statement *as;
            bool is_lock;
        } import_file;
        struct {
            struct Statement *file;
            struct Parameter *pt;
            struct Parameter *as;
            bool is_lock;
        } from_import_file;
        struct {
            struct Parameter *var;
            enum DefaultType{
                default_,
                global_,
                nonlocal_,
            } default_type;
        } default_var;
        struct {
            struct Statement *conditions;
        } assert;
        struct {
            struct Statement *command;
            struct Statement *as;
            wchar_t *label;
        } label_;
        struct {
            struct Statement *times;
            struct Statement *return_;
            struct Statement *label;
        } goto_;
    }u;
    struct {  // 运行info信息
        bool have_info;
        struct VarList *var_list;
        struct Statement *node;
        struct {
            struct StatementList *sl_node;
            enum StatementInfoStatus{
                info_vl_branch,
                info_else_branch,
                info_finally_branch,
                info_first_do,
                info_after_do,
            } status;
            struct{
                LinkValue *value;
                LinkValue *_exit_;
                LinkValue *_enter_;
                LinkValue *with_belong;
            } with_;
            struct{
                LinkValue *iter;
            } for_;
        } branch;
    } info;
    fline line;
    char *code_file;
    struct Statement *next;
};

struct StatementList{
    enum {
        if_b,
        do_b,
        while_b,
        except_b,
        for_b,
        with_b,
    } type;
    struct Statement *condition;
    struct Statement *var;
    struct Statement *code;
    struct StatementList *next;
};

struct DecorationStatement {
    struct Statement *decoration;
    struct DecorationStatement *next;
};

typedef struct Token Token;
typedef struct Statement Statement;
typedef struct StatementList StatementList;
typedef struct DecorationStatement DecorationStatement;

Statement *makeStatement(fline line, char *file);
void setRunInfo(Statement *st);
void freeRunInfo(Statement *st);
void freeStatement(Statement *st);
Statement *copyStatement(Statement *st);
Statement *copyStatementCore(Statement *st);
void connectStatement(Statement *base, Statement *new);

Statement *makeOperationBaseStatement(enum OperationType type, fline line, char *file);
Statement *makeOperationStatement(enum OperationType type, Statement *left, Statement *right);
Statement *makeBaseLinkValueStatement(LinkValue *value, fline line, char *file);
Statement *makeBaseStrValueStatement(wchar_t *value, enum BaseValueType type, fline line, char *file);
Statement *makeBaseValueStatement(enum BaseValueType type, fline line, char *file);
Statement *makeBaseVarStatement(wchar_t *name, Statement *times, fline line, char *file);
Statement *makeBaseSVarStatement(Statement *name, Statement *times);
Statement *makeBaseDictStatement(Parameter *pt, fline line, char *file);
Statement *makeTupleStatement(Parameter *pt, enum ListType type, fline line, char *file);
Statement *makeClassStatement(Statement *name, Statement *function, Parameter *pt);
Statement *makeFunctionStatement(Statement *name, Statement *function, struct Parameter *pt);
Statement *makeLambdaStatement(Statement *function, Parameter *pt);
Statement *makeCallStatement(Statement *function, struct Parameter *pt);
Statement *makeSliceStatement(Statement *element, Parameter *index, enum SliceType type);
Statement *makeForStatement(fline line, char *file);
Statement *makeIfStatement(fline line, char *file);
Statement *makeWhileStatement(fline line, char *file);
Statement *makeTryStatement(fline line, char *file);
Statement *makeBreakStatement(Statement *times, fline line, char *file);
Statement *makeWithStatement(fline line, char *file);
Statement *makeContinueStatement(Statement *times, fline line, char *file);
Statement *makeRegoStatement(Statement *times, fline line, char *file);
Statement *makeRestartStatement(Statement *times, fline line, char *file);
Statement *makeReturnStatement(Statement *value, fline line, char *file);
Statement *makeYieldStatement(Statement *value, fline line, char *file);
Statement *makeRaiseStatement(Statement *value, fline line, char *file);
Statement *makeAssertStatement(Statement *conditions, fline line, char *file);
Statement *makeIncludeStatement(Statement *file, fline line, char *file_dir);
Statement *makeImportStatement(Statement *file, Statement *as, bool is_lock);
Statement *makeFromImportStatement(Statement *file, Parameter *as, Parameter *pt, bool is_lock);
Statement *makeDefaultVarStatement(Parameter *var, fline line, char *file_dir, enum DefaultType type);
Statement *makeLabelStatement(Statement *var, Statement *command, wchar_t *label, fline line, char *file_dir);
Statement *makeGotoStatement(Statement *return_, Statement *times, Statement *label, fline line, char *file_dir);
Statement *makeDelStatement(Statement *var, fline line, char *file_dir);
Token *setOperationFromToken(Statement **st_ad, Token *left, Token *right, enum OperationType type, bool is_right);

StatementList *makeStatementList(Statement *condition, Statement *var, Statement *code, int type);
StatementList *connectStatementList(StatementList *base, StatementList *new);
void freeStatementList(StatementList *base);
StatementList *copyStatementList(StatementList *sl);

DecorationStatement *makeDecorationStatement();
DecorationStatement *connectDecorationStatement(Statement *decoration, DecorationStatement *base);
void freeDecorationStatement(DecorationStatement *base);
DecorationStatement *copyDecorationStatement(DecorationStatement *ds);
DecorationStatement *copyDecorationStatementCore(DecorationStatement *base);

#endif //VIRTUALMATH_STATEMENT_H
