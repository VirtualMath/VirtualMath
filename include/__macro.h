#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>

// 布尔逻辑的定义
#define bool int
#define true 1
#define false 0

// PASS语句的定义
#define PASS do{}while(0)

#define NUMBER_TYPE long int
#define NUMBER_FORMAT "ld"
#define HASH_INDEX unsigned int

#define INTER_FUNCTIONSIG_CORE struct Inter *inter, struct VarList *var_list
#define INTER_FUNCTIONSIG Statement *st, INTER_FUNCTIONSIG_CORE
#define CALL_INTER_FUNCTIONSIG_CORE(var_list) inter, var_list
#define CALL_INTER_FUNCTIONSIG(st, var_list) st, CALL_INTER_FUNCTIONSIG_CORE(var_list)

#define run_continue(result) (result.type == not_return || result.type == operation_return)
#define run_continue_(result) (result->type == not_return || result->type == operation_return)
#define is_error(result) (result.type == error_return)

#define freeBase(element, return_) do{ \
if (element == NULL){ \
goto return_; \
} \
}while(0) \


#define checkResult(check_result) do{ \
if (is_error(check_result)){ \
return check_result; \
} \
}while(0) \

#endif //VIRTUALMATH___MACRO_H
