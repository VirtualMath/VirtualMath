#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H
#include "macro.h"

// PASS语句的定义
#define PASS

#define RUN_TYPE(type) (type == R_not || type == R_opt)
#define CHECK_RESULT(result) (result->type == R_not || result->type == R_opt)

#define FREE_BASE(element, return_) do{if(element == NULL)goto return_;}while(0)

#define FUNC_CORE struct Inter *inter, struct VarList *var_list
#define FUNC_NT struct LinkValue *belong, struct Result *result, FUNC_CORE
#define FUNC struct Statement *st, FUNC_NT

#define CFUNC_CORE(var_list) inter, var_list
#define CFUNC_NT(var_list, result, belong) belong, result, CFUNC_CORE(var_list)
#define CFUNC(st, var_list, result, belong) st, CFUNC_NT(var_list, result, belong)
#define CNEXT CFUNC(st, var_list, result, belong)
#define CNEXT_NT CFUNC_NT(var_list, result, belong)

#define O_FUNC struct Argument *arg, FUNC_NT
#define CO_FUNC(arg, var_list, result, belong) arg, CFUNC_NT(var_list, result, belong)
#define R_FUNC struct LinkValue *belong, FUNC_CORE
#define CR_FUNC(belong, var_list) belong, CFUNC_CORE(var_list)

#define LINEFILE __LINE__, __FILE__

#define MD5_SIZE (16)
#define MD5_STR_LEN (MD5_SIZE * 2)
#define MD5_STRING (MD5_STR_LEN + 1)

typedef long long vint;
typedef long double vdou;
typedef unsigned long long vhashn;
typedef unsigned long long fline;

#endif //VIRTUALMATH___MACRO_H
