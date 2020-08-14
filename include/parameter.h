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

typedef struct Parameter Parameter;
typedef struct Argument Argument;

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
Argument *listToArgument(LinkValue *list_value, INTER_FUNCTIONSIG_CORE);
Argument *dictToArgument(LinkValue *dict_value, INTER_FUNCTIONSIG_CORE);

ResultType setParameterCore(Argument *call, Parameter *function_base, VarList *function_var, INTER_FUNCTIONSIG_NOT_ST);
ResultType setParameter(Parameter *call_base, Parameter *function_base, VarList *function_var, LinkValue *function_father, INTER_FUNCTIONSIG_NOT_ST);
ResultType iterParameter(Parameter *call, Argument **base_ad, bool is_dict, INTER_FUNCTIONSIG_NOT_ST);
Argument * getArgument(Parameter *call, bool is_dict, INTER_FUNCTIONSIG_NOT_ST);

ResultType defaultParameter(Parameter **function_ad, NUMBER_TYPE *num, INTER_FUNCTIONSIG_NOT_ST);
ResultType argumentToVar(Argument **call_ad, NUMBER_TYPE *num, INTER_FUNCTIONSIG_NOT_ST);
ResultType parameterFromVar(Parameter **function_ad, VarList *function_var, NUMBER_TYPE *num, NUMBER_TYPE max, bool *status,
                        INTER_FUNCTIONSIG_NOT_ST);
ResultType argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_NOT_ST);

FatherValue *setFatherCore(FatherValue *father_tmp);
FatherValue *setFather(Argument *call, INTER_FUNCTIONSIG_NOT_ST);
#endif //VIRTUALMATH_PARAMETER_H
