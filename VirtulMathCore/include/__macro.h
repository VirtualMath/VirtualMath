#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H
#include "macro.h"

// PASS语句的定义
#define PASS

#define RUN_TYPE(type) (type == not_return || type == operation_return)
#define CHECK_RESULT(result) (result->type == not_return || result->type == operation_return)

#define FREE_BASE(element, return_) do{if(element == NULL)goto return_;}while(0)

#define INTER_FUNCTIONSIG_CORE struct Inter *inter, struct VarList *var_list
#define INTER_FUNCTIONSIG_NOT_ST struct LinkValue *belong, struct Result *result, INTER_FUNCTIONSIG_CORE
#define INTER_FUNCTIONSIG struct Statement *st, INTER_FUNCTIONSIG_NOT_ST

#define CALL_INTER_FUNCTIONSIG_CORE(var_list) inter, var_list
#define CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong) belong, result, CALL_INTER_FUNCTIONSIG_CORE(var_list)
#define CALL_INTER_FUNCTIONSIG(st, var_list, result, belong) st, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)

#define OFFICAL_FUNCTIONSIG struct Argument *arg, INTER_FUNCTIONSIG_NOT_ST
#define CALL_OFFICAL_FUNCTION(arg, var_list, result, belong) arg, CALL_INTER_FUNCTIONSIG_NOT_ST(var_list, result, belong)
#define REGISTERED_FUNCTIONSIG struct LinkValue *belong, INTER_FUNCTIONSIG_CORE
#define CALL_REGISTERED_FUNCTION(belong, var_list) belong, CALL_INTER_FUNCTIONSIG_CORE(var_list)

#define MD5_SIZE (16)
#define MD5_STR_LEN (MD5_SIZE * 2)
#define MD5_STRING (MD5_STR_LEN + 1)

typedef long long vnum;
typedef unsigned long long vhashn;
typedef unsigned long long fline;

#endif //VIRTUALMATH___MACRO_H
