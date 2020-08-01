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

Parameter *makeParameter();
Parameter *makeOnlyValueParameter(struct Statement *st);
Parameter *makeNameValueParameter(struct Statement *value, struct Statement *name);
Parameter *makeOnlyArgsParameter(Statement *st);
Parameter *connectParameter(Parameter *new, Parameter *base);
Parameter *connectOnlyValueParameter(struct Statement *st, Parameter *base);
Parameter *connectNameValueParameter(struct Statement *value, struct Statement *name, Parameter *base);
Parameter *connectOnlyArgsParameter(Statement *st, Parameter *base);
void freeParameter(Parameter *pt);
struct VirtualMathResult setParameter(Parameter *call, Parameter *function, struct VirtualMathVarList *function_var, INTER_FUNCTIONSIG_CORE);
#endif //VIRTUALMATH_PARAMETER_H
