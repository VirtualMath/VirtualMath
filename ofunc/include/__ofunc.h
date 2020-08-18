#ifndef VIRTUALMATH___OFUNC_H
#define VIRTUALMATH___OFUNC_H
#include "__virtualmath.h"

void registeredFunctionCore(OfficialFunction of, char *name, struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
void iterNameFunc(NameFunc list[],struct LinkValue *father, INTER_FUNCTIONSIG_CORE);

#endif //VIRTUALMATH___OFUNC_H
