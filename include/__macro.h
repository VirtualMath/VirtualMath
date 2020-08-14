#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H
#include "macro.h"

// PASS语句的定义
#define PASS

#define NUMBER_TYPE long int
#define NUMBER_FORMAT "ld"
#define HASH_INDEX unsigned int

#define INTER_FUNCTIONSIG_CORE struct Inter *inter, struct VarList *var_list
#define INTER_FUNCTIONSIG_NOT_ST struct LinkValue *father, struct Result *result, INTER_FUNCTIONSIG_CORE
#define INTER_FUNCTIONSIG struct Statement *st, INTER_FUNCTIONSIG_NOT_ST

#define CALL_INTER_FUNCTIONSIG_CORE(var_list) inter, var_list
#define CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father) father, result, CALL_INTER_FUNCTIONSIG_CORE(var_list)
#define CALL_INTER_FUNCTIONSIG(st, var_list, result, father) st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, father)

#define run_continue_type(type) (type == not_return || type == operation_return)
#define run_continue(result) (result->type == not_return || result->type == operation_return)

#define freeBase(element, return_) do{ \
if (element == NULL){ \
goto return_; \
} \
}while(0) \


#endif //VIRTUALMATH___MACRO_H
