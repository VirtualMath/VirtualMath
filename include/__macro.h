#ifndef VIRTUALMATH___MACRO_H
#define VIRTUALMATH___MACRO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define bool int
#define true 1
#define false 0

#define PASS ;

#define NUMBER_TYPE long int
#define HASH_INDEX unsigned int
#define INTER_FUNCTIONSIG Statement *st, Inter *inter, VarList *var_list
#define CALL_INTER_FUNCTIONSIG(st, var_list) st, inter, var_list

#endif //VIRTUALMATH___MACRO_H
