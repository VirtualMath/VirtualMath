#ifndef VIRTUALMATH_PARAMETER_H
#define VIRTUALMATH_PARAMETER_H
#include "__macro.h"

typedef struct Parameter{
    enum {
        only_value,
        name_value,
    } type;
    struct {
        struct Statement *value;
        struct Statement *name;  // 仅在name-value模式生效
    } data;
    struct Parameter *next;
} Parameter;

Parameter *makeParameter();
Parameter *makeOnlyValueParameter(struct Statement *st);
Parameter *makeNameValueParameter(Statement *value, Statement *name);
Parameter *connectParameter(Parameter *new, Parameter *base);
Parameter *connectOnlyValueParameter(struct Statement *st, Parameter *base);
Parameter *connectNameValueParameter(Statement *value, Statement *name, Parameter *base);
void freeParameter(Parameter *pt);
Result setParameter(Parameter *call, Parameter *function, VarList *function_var, INTER_FUNCTIONSIG_CORE);
#endif //VIRTUALMATH_PARAMETER_H
