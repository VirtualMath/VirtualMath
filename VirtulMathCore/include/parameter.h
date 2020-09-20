#ifndef VIRTUALMATH_PARAMETER_H
#define VIRTUALMATH_PARAMETER_H

#include "__macro.h"

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
        char *name_;
        struct LinkValue *name_value;
    } data;
    struct Argument *next;
};

struct ArgumentParser{
    struct LinkValue *value;
    struct Argument *arg;
    char *name;
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
Argument *makeCharNameArgument(LinkValue *value, LinkValue *name_value, char *name);
Argument *connectArgument(Argument *new, Argument *base);
Argument *connectValueArgument(LinkValue *value, Argument *base);
Argument *connectStatementNameArgument(LinkValue *value, struct Statement *name, Argument *base);
Argument *connectCharNameArgument(LinkValue *value, LinkValue *name_value, char *name, Argument *base);
void freeArgument(Argument *at, bool free_st);

Parameter *makeParameter(void);
Parameter *copyParameter(Parameter *base);
Parameter *makeValueParameter(struct Statement *st);
Parameter *makeNameParameter(struct Statement *value, struct Statement *name);
Parameter *makeArgsParameter(struct Statement *st);
Parameter *makeKwrgsParameter(struct Statement *st);
Parameter *connectParameter(Parameter *new, Parameter *base);
Parameter *connectValueParameter(struct Statement *st, Parameter *base);
Parameter *connectNameParameter(struct Statement *value, struct Statement *name, Parameter *base);
Parameter *connectArgsParameter(struct Statement *st, Parameter *base);
Parameter *connectKwargsParameter(struct Statement *st, Parameter *base);
void freeParameter(Parameter *pt, bool free_st);
Argument *listToArgument(LinkValue *list_value, long line, char *file, INTER_FUNCTIONSIG_NOT_ST);
Argument *dictToArgument(LinkValue *dict_value, long line, char *file, INTER_FUNCTIONSIG_NOT_ST);

ResultType setParameterCore(fline line, char *file, Argument *call, Parameter *function_base, VarList *function_var, INTER_FUNCTIONSIG_NOT_ST);
ResultType iterParameter(Parameter *call, Argument **base_ad, bool is_dict, INTER_FUNCTIONSIG_NOT_ST);
Argument *getArgument(Parameter *call, bool is_dict, INTER_FUNCTIONSIG_NOT_ST);

ResultType defaultParameter(Parameter **function_ad, vnum *num, INTER_FUNCTIONSIG_NOT_ST);
ResultType argumentToVar(Argument **call_ad, vnum *num, INTER_FUNCTIONSIG_NOT_ST);
ResultType parameterFromVar(Parameter **function_ad, VarList *function_var, vnum *num, vnum max, bool *status,
                            INTER_FUNCTIONSIG_NOT_ST);
ResultType argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_NOT_ST);

Inherit *setFatherCore(Inherit *father_tmp);
Inherit *setFather(Argument *call);
bool checkFormal(Parameter *pt);

Argument *parserValueArgument(ArgumentParser *ap, Argument *arg, int *status, ArgumentParser **bak);
int parserNameArgument(ArgumentParser ap[], Argument *arg, ArgumentParser **bak, INTER_FUNCTIONSIG_NOT_ST);
int parserArgumentUnion(ArgumentParser ap[], Argument *arg, INTER_FUNCTIONSIG_NOT_ST);
Argument *parserArgumentValueCore(Argument *arg, ArgumentParser *ap);
ArgumentParser *parserArgumentNameDefault(ArgumentParser *ap);
ArgumentParser *parserArgumentValueDefault(ArgumentParser *ap);
int parserArgumentVar(ArgumentParser *ap, Inter *inter, VarList *var_list);
#endif //VIRTUALMATH_PARAMETER_H
