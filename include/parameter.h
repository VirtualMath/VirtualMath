#ifndef VIRTUALMATH_PARAMETER_H
#define VIRTUALMATH_PARAMETER_H
#include "__macro.h"

struct Parameter{
    enum ParameterType{
        value_par,
        name_par,
        args_par,
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
    struct ArgumentData{
        struct LinkValue *value;
        struct Statement *name;  // 仅在name-value模式生效
    } data;
    struct Argument *next;
};

typedef struct Parameter Parameter;
typedef struct Argument Argument;

Argument *makeArgument();
Argument *makeOnlyValueArgument(LinkValue *value);
Argument *makeNameValueArgument(LinkValue *value, struct Statement *name);
Argument *connectArgument(Argument *new, Argument *base);
Argument *connectOnlyValueArgument(LinkValue *value, Argument *base);
Argument *connectNameValueArgument(LinkValue *value, struct Statement *name, Argument *base);
void freeArgument(Argument *pt, bool free_st);

Parameter *makeParameter();
Parameter *copyParameter(Parameter *base);
Parameter *makeOnlyValueParameter(struct Statement *st);
Parameter *makeNameValueParameter(struct Statement *value, struct Statement *name);
Parameter *makeOnlyArgsParameter(struct Statement *st);
Parameter *connectParameter(Parameter *new, Parameter *base);
Parameter *connectOnlyValueParameter(struct Statement *st, Parameter *base);
Parameter *connectNameValueParameter(struct Statement *value, struct Statement *name, Parameter *base);
Parameter *connectOnlyArgsParameter(struct Statement *st, Parameter *base);
void freeParameter(Parameter *pt, bool free_st);

Result setParameterCore(Argument *call, Parameter *function_base, VarList *function_var, INTER_FUNCTIONSIG_CORE);
Result setParameter(Parameter *call, Parameter *function, VarList *function_var, INTER_FUNCTIONSIG_CORE);
Result iterParameter(Parameter *call, Argument **base_ad, INTER_FUNCTIONSIG_CORE);
Argument *getArgument(Parameter *call, Result *result, INTER_FUNCTIONSIG_CORE);
#endif //VIRTUALMATH_PARAMETER_H
