#ifndef VIRTUALMATH_PARAMETER_H
#define VIRTUALMATH_PARAMETER_H

#include "__macro.h"
#include "inter.h"

struct Parameter{
    enum ParameterType{
        value_par,
        name_par,
        args_par,
        kwargs_par,
    } type;
    struct ParameterData{
        struct Statement *value;
        struct Statement *name;  // 仅在name-value模式生效
        bool is_sep;  // 是否为分割符
    } data;
    struct Parameter *next;
};

struct Argument{
    enum ArgumentType{
        value_arg,
        name_arg,
    } type;
    enum ArgumentNameType{
        name_st,
        name_char,
    } name_type;
    struct ArgumentData{
        struct LinkValue *value;
        struct Statement *name;  // 仅在name-value模式生效
        wchar_t *name_;
        struct LinkValue *name_value;
    } data;
    struct Argument *next;
};

struct ArgumentParser{
    struct LinkValue *value;
    struct Argument *arg;
    wchar_t *name;
    enum ArgumentParserType{
        only_value,
        name_value,
        only_name,
    } type;
    int must;
    bool long_arg;
    int c_count;
};

typedef struct Parameter Parameter;
typedef struct Argument Argument;
typedef struct ArgumentParser ArgumentParser;

Argument *makeArgument(void);
Argument *makeValueArgument(LinkValue *value);
Argument *makeStatementNameArgument(LinkValue *value, struct Statement *name);
Argument *makeCharNameArgument(LinkValue *value, LinkValue *name_value, wchar_t *name);
Argument *connectArgument(Argument *new, Argument *base);
Argument *connectValueArgument(LinkValue *value, Argument *base);
Argument *connectStatementNameArgument(LinkValue *value, struct Statement *name, Argument *base);
Argument *connectCharNameArgument(LinkValue *value, LinkValue *name_value, wchar_t *name, Argument *base);
void freeArgument(Argument *at, bool free_st);

Parameter *makeParameter(void);
Parameter *copyParameter(Parameter *base);
Parameter *makeValueParameter(struct Statement *st);
Parameter *makeNameParameter(struct Statement *value, struct Statement *name);
Parameter *makeArgsParameter(struct Statement *st);
Parameter *makeKwrgsParameter(struct Statement *st);
Parameter *connectParameter(Parameter *new, Parameter *base);
Parameter *connectValueParameter(Statement *st, Parameter *base, bool is_sep);
Parameter *connectNameParameter(struct Statement *value, struct Statement *name, Parameter *base);
Parameter *connectArgsParameter(Statement *st, Parameter *base, bool is_sep);
Parameter *connectKwargsParameter(struct Statement *st, Parameter *base);
void freeParameter(Parameter *pt, bool free_st);
Argument *listToArgument(LinkValue *list_value, long line, char *file, FUNC_NT);
Argument *dictToArgument(LinkValue *dict_value, long line, char *file, FUNC_NT);

ResultType setParameterCore(fline line, char *file, Argument *call, Parameter *function_base, VarList *function_var, FUNC_NT);
ResultType iterParameter(Parameter *call, Argument **base_ad, bool is_dict, FUNC_NT);
Argument *getArgument(Parameter *call, bool is_dict, FUNC_NT);

ResultType defaultParameter(Parameter **function_ad, vnum *num, FUNC_NT);
ResultType argumentToVar(Argument **call_ad, vnum *num, FUNC_NT);
ResultType parameterFromVar(Parameter **function_ad, VarList *function_var, vnum *num, vnum max, bool *status,
                            FUNC_NT);
ResultType argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, FUNC_NT);

Inherit *setFatherCore(Inherit *father_tmp);
Inherit *setFather(Argument *call);
bool checkFormal(Parameter *pt);

Argument *parserValueArgument(ArgumentParser *ap, Argument *arg, int *status, ArgumentParser **bak);
int parserNameArgument(ArgumentParser ap[], Argument *arg, ArgumentParser **bak, FUNC_NT);
int parserArgumentUnion(ArgumentParser ap[], Argument *arg, FUNC_NT);
Argument *parserArgumentValueCore(Argument *arg, ArgumentParser *ap);
ArgumentParser *parserArgumentNameDefault(ArgumentParser *ap);
ArgumentParser *parserArgumentValueDefault(ArgumentParser *ap);
int parserArgumentVar(ArgumentParser *ap, Inter *inter, VarList *var_list);
#endif //VIRTUALMATH_PARAMETER_H
