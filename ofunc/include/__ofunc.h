#ifndef VIRTUALMATH___OFUNC_H
#define VIRTUALMATH___OFUNC_H
#include "__virtualmath.h"
#include "__base.h"
#include "__run.h"


LinkValue *registeredFunctionCore(OfficialFunction of, char *name, struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
void iterNameFunc(NameFunc list[],struct LinkValue *father, INTER_FUNCTIONSIG_CORE);
Value *makeBaseChildClass(Value *father, Inter *inter);

#endif //VIRTUALMATH___OFUNC_H
