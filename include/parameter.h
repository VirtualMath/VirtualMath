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
    } data;
    struct Argument *next;
};

typedef struct Parameter Parameter;
typedef struct Argument Argument;

Argument *makeArgument();
Argument *makeOnlyValueArgument(LinkValue *value);
Argument *makeNameValueArgument(LinkValue *value, struct Statement *name);
Argument *makeCharNameArgument(LinkValue *value, char *name);
Argument *connectArgument(Argument *new, Argument *base);
Argument *connectOnlyValueArgument(LinkValue *value, Argument *base);
Argument *connectNameValueArgument(LinkValue *value, struct Statement *name, Argument *base);
Argument *connectCharNameArgument(LinkValue *value, char *name, Argument *base);
void freeArgument(Argument *at, bool free_st);

Parameter *makeParameter();
Parameter *copyParameter(Parameter *base);
Parameter *makeOnlyValueParameter(struct Statement *st);
Parameter *makeNameValueParameter(struct Statement *value, struct Statement *name);
Parameter *makeOnlyArgsParameter(struct Statement *st);
Parameter *makeNameArgsParameter(struct Statement *st);
Parameter *connectParameter(Parameter *new, Parameter *base);
Parameter *connectOnlyValueParameter(struct Statement *st, Parameter *base);
Parameter *connectNameValueParameter(struct Statement *value, struct Statement *name, Parameter *base);
Parameter *connectOnlyArgsParameter(struct Statement *st, Parameter *base);
Parameter *connectNameArgsParameter(struct Statement *st, Parameter *base);
void freeParameter(Parameter *pt, bool free_st);
Argument *listToArgument(LinkValue *list_value, INTER_FUNCTIONSIG_CORE);
Argument *dictToArgument(LinkValue *dict_value, INTER_FUNCTIONSIG_CORE);

Result setParameterCore(Argument *call, Parameter *function_base, VarList *function_var, INTER_FUNCTIONSIG_CORE);
Result setParameter(Parameter *call, Parameter *function, VarList *function_var, INTER_FUNCTIONSIG_CORE);
Result iterParameter(Parameter *call, Argument **base_ad, INTER_FUNCTIONSIG_CORE);
Argument *getArgument(Parameter *call, Result *result, INTER_FUNCTIONSIG_CORE);

Result defaultParameter(Parameter **function_ad, Inter *inter, VarList *var_list, int *num);
Result argumentToVar(Argument **call_ad, struct Inter *inter, struct VarList *var_list, NUMBER_TYPE *num);
Result parameterFromVar(Parameter **function_ad, VarList *function_var, struct Inter *inter, struct VarList *var_list,
                        NUMBER_TYPE *num, NUMBER_TYPE max, bool *status);
Result argumentToParameter(Argument **call_ad, Parameter **function_ad, VarList *function_var, INTER_FUNCTIONSIG_CORE);
#endif //VIRTUALMATH_PARAMETER_H
