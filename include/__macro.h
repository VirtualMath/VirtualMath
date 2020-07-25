#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define bool int
#define true 1
#define false 0

#define pass ;

#define number_type long int
#define baseFunctionSig Statement *st, Inter *inter, VarList *var_list
#define callFunctionSig(st, var_list) st, inter, var_list

#endif //VIRTUALMATH___MACRO_H
