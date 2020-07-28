#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define bool int
#define true 1
#define false 0

#define PASS do{}while(0)

#define NUMBER_TYPE long int
#define HASH_INDEX unsigned int
#define INTER_FUNCTIONSIG_CORE Inter *inter, VarList *var_list
#define INTER_FUNCTIONSIG Statement *st, INTER_FUNCTIONSIG_CORE
#define CALL_INTER_FUNCTIONSIG_CORE(var_list) inter, var_list
#define CALL_INTER_FUNCTIONSIG(st, var_list) st, CALL_INTER_FUNCTIONSIG_CORE(var_list)

#define freeBase(element, return_) do{ \
if (element == NULL){ \
goto return_; \
} \
}while(0) \

#define runContinue(result) (result.type == not_return || result.type == operation_return)
#define is_error(result) (result.type == error_return)

#define safeIterStatement(new_result, sig) do{ \
new_result = iterStatement(sig); \
if (is_error(new_result)){ \
return new_result; \
} \
} while(0)

#define checkResult(check_result) do{ \
if (is_error(check_result)){ \
return check_result; \
} \
}while(0) \

#endif //VIRTUALMATH___MACRO_H
