#ifndef VIRTUALMATH_RUN_H
#define VIRTUALMATH_RUN_H
#include "statement.h"

typedef struct globalInterpreter{
    struct VirtualMathValue *base;
    struct VirtualMathLinkValue *link_base;
    struct VirtualMathHashTable *hash_base;
    Statement *statement;
    VarList *var_list;
} Inter;


#endif //VIRTUALMATH_RUN_H
