#ifndef VIRTUALMATH_PARAMETER_H
#define VIRTUALMATH_PARAMETER_H
#include "__macro.h"

typedef struct Parameter{
    enum {
        only_value,
        name_value,
        only_args,
    } type;
    struct {
        struct Statement *value;
        struct Statement *name;  // 仅在name-value模式生效
    } data;
    struct Parameter *next;
} Parameter;

typedef struct Argument{
    enum {
        value_arg,
        name_arg,
    } type;
    struct {
        struct LinkValue *value;
        struct Statement *name;  // 仅在name-value模式生效
    } data;
    struct Argument *next;
} Argument;

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

Result setParameter(Parameter *call, Parameter *function, VarList *function_var, INTER_FUNCTIONSIG_CORE);
Result iterParameter(Parameter *call, Argument **base_ad, INTER_FUNCTIONSIG_CORE);
Argument *getArgument(Parameter *call, Result *result, INTER_FUNCTIONSIG_CORE);
#endif //VIRTUALMATH_PARAMETER_H
